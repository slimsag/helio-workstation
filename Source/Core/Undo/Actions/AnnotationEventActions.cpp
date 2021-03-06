/*
    This file is part of Helio Workstation.

    Helio is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Helio is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Helio. If not, see <http://www.gnu.org/licenses/>.
*/

#include "Common.h"
#include "AnnotationEventActions.h"
#include "AnnotationsSequence.h"
#include "ProjectTreeItem.h"
#include "SerializationKeys.h"


//===----------------------------------------------------------------------===//
// Insert
//===----------------------------------------------------------------------===//

AnnotationEventInsertAction::AnnotationEventInsertAction(ProjectTreeItem &parentProject,
                                                         String targetTrackId,
                                                         const AnnotationEvent &event) :
    UndoAction(parentProject),
    trackId(std::move(targetTrackId)),
    event(event)
{
}

bool AnnotationEventInsertAction::perform()
{
    if (AnnotationsSequence *sequence =
        this->project.findSequenceByTrackId<AnnotationsSequence>(this->trackId))
    {
        return (sequence->insert(this->event, false) != nullptr);
    }
    
    return false;
}

bool AnnotationEventInsertAction::undo()
{
    if (AnnotationsSequence *sequence =
        this->project.findSequenceByTrackId<AnnotationsSequence>(this->trackId))
    {
        return sequence->remove(this->event, false);
    }
    
    return false;
}

int AnnotationEventInsertAction::getSizeInUnits()
{
    return sizeof(AnnotationEvent);
}

XmlElement *AnnotationEventInsertAction::serialize() const
{
    auto xml = new XmlElement(Serialization::Undo::annotationEventInsertAction);
    xml->setAttribute(Serialization::Undo::trackId, this->trackId);
    xml->prependChildElement(this->event.serialize());
    return xml;
}

void AnnotationEventInsertAction::deserialize(const XmlElement &xml)
{
    this->trackId = xml.getStringAttribute(Serialization::Undo::trackId);
    this->event.deserialize(*xml.getFirstChildElement());
}

void AnnotationEventInsertAction::reset()
{
    this->event.reset();
    this->trackId.clear();
}


//===----------------------------------------------------------------------===//
// Remove
//===----------------------------------------------------------------------===//

AnnotationEventRemoveAction::AnnotationEventRemoveAction(ProjectTreeItem &parentProject,
                                                         String targetTrackId,
                                                         const AnnotationEvent &target) :
    UndoAction(parentProject),
    trackId(std::move(targetTrackId)),
    event(target)
{
}

bool AnnotationEventRemoveAction::perform()
{
    if (AnnotationsSequence *sequence =
        this->project.findSequenceByTrackId<AnnotationsSequence>(this->trackId))
    {
        return sequence->remove(this->event, false);
    }
    
    return false;
}

bool AnnotationEventRemoveAction::undo()
{
    if (AnnotationsSequence *sequence =
        this->project.findSequenceByTrackId<AnnotationsSequence>(this->trackId))
    {
        return (sequence->insert(this->event, false) != nullptr);
    }
    
    return false;
}

int AnnotationEventRemoveAction::getSizeInUnits()
{
    return sizeof(AnnotationEvent);
}

XmlElement *AnnotationEventRemoveAction::serialize() const
{
    auto xml = new XmlElement(Serialization::Undo::annotationEventRemoveAction);
    xml->setAttribute(Serialization::Undo::trackId, this->trackId);
    xml->prependChildElement(this->event.serialize());
    return xml;
}

void AnnotationEventRemoveAction::deserialize(const XmlElement &xml)
{
    this->trackId = xml.getStringAttribute(Serialization::Undo::trackId);
    this->event.deserialize(*xml.getFirstChildElement());
}

void AnnotationEventRemoveAction::reset()
{
    this->event.reset();
    this->trackId.clear();
}


//===----------------------------------------------------------------------===//
// Change
//===----------------------------------------------------------------------===//

AnnotationEventChangeAction::AnnotationEventChangeAction(ProjectTreeItem &parentProject,
                                                         String targetTrackId,
                                                         const AnnotationEvent &target,
                                                         const AnnotationEvent &newParameters) :
    UndoAction(parentProject),
    trackId(std::move(targetTrackId)),
    eventBefore(target),
    eventAfter(newParameters)
{
}

bool AnnotationEventChangeAction::perform()
{
    if (AnnotationsSequence *sequence =
        this->project.findSequenceByTrackId<AnnotationsSequence>(this->trackId))
    {
        return sequence->change(this->eventBefore, this->eventAfter, false);
    }
    
    return false;
}

bool AnnotationEventChangeAction::undo()
{
    if (AnnotationsSequence *sequence =
        this->project.findSequenceByTrackId<AnnotationsSequence>(this->trackId))
    {
        return sequence->change(this->eventAfter, this->eventBefore, false);
    }
    
    return false;
}

int AnnotationEventChangeAction::getSizeInUnits()
{
    return sizeof(AnnotationEvent) * 2;
}

UndoAction *AnnotationEventChangeAction::createCoalescedAction(UndoAction *nextAction)
{
    if (AnnotationsSequence *sequence =
        this->project.findSequenceByTrackId<AnnotationsSequence>(this->trackId))
    {
        if (AnnotationEventChangeAction *nextChanger =
            dynamic_cast<AnnotationEventChangeAction *>(nextAction))
        {
            const bool idsAreEqual =
                (this->eventBefore.getId() == nextChanger->eventAfter.getId() &&
                    this->trackId == nextChanger->trackId);
            
            if (idsAreEqual)
            {
                return new AnnotationEventChangeAction(this->project,
                    this->trackId, this->eventBefore, nextChanger->eventAfter);
            }
        }
    }

    (void) nextAction;
    return nullptr;
}

XmlElement *AnnotationEventChangeAction::serialize() const
{
    auto xml = new XmlElement(Serialization::Undo::annotationEventChangeAction);
    xml->setAttribute(Serialization::Undo::trackId, this->trackId);
    
    auto annotationBeforeChild = new XmlElement(Serialization::Undo::annotationBefore);
    annotationBeforeChild->prependChildElement(this->eventBefore.serialize());
    xml->prependChildElement(annotationBeforeChild);
    
    auto annotationAfterChild = new XmlElement(Serialization::Undo::annotationAfter);
    annotationAfterChild->prependChildElement(this->eventAfter.serialize());
    xml->prependChildElement(annotationAfterChild);
    
    return xml;
}

void AnnotationEventChangeAction::deserialize(const XmlElement &xml)
{
    this->trackId = xml.getStringAttribute(Serialization::Undo::trackId);
    
    XmlElement *annotationBeforeChild = xml.getChildByName(Serialization::Undo::annotationBefore);
    XmlElement *annotationAfterChild = xml.getChildByName(Serialization::Undo::annotationAfter);
    
    this->eventBefore.deserialize(*annotationBeforeChild->getFirstChildElement());
    this->eventAfter.deserialize(*annotationAfterChild->getFirstChildElement());
}

void AnnotationEventChangeAction::reset()
{
    this->eventBefore.reset();
    this->eventAfter.reset();
    this->trackId.clear();
}


//===----------------------------------------------------------------------===//
// Insert Group
//===----------------------------------------------------------------------===//

AnnotationEventsGroupInsertAction::AnnotationEventsGroupInsertAction(ProjectTreeItem &parentProject,
                                                                     String targetTrackId,
                                                                     Array<AnnotationEvent> &target) :
    UndoAction(parentProject),
    trackId(std::move(targetTrackId))
{
    this->annotations.swapWith(target);
}

bool AnnotationEventsGroupInsertAction::perform()
{
    if (AnnotationsSequence *sequence =
        this->project.findSequenceByTrackId<AnnotationsSequence>(this->trackId))
    {
        return sequence->insertGroup(this->annotations, false);
    }
    
    return false;
}

bool AnnotationEventsGroupInsertAction::undo()
{
    if (AnnotationsSequence *sequence =
        this->project.findSequenceByTrackId<AnnotationsSequence>(this->trackId))
    {
        return sequence->removeGroup(this->annotations, false);
    }
    
    return false;
}

int AnnotationEventsGroupInsertAction::getSizeInUnits()
{
    return (sizeof(AnnotationEvent) * this->annotations.size());
}

XmlElement *AnnotationEventsGroupInsertAction::serialize() const
{
    auto xml = new XmlElement(Serialization::Undo::annotationEventsGroupInsertAction);
    xml->setAttribute(Serialization::Undo::trackId, this->trackId);
    
    for (int i = 0; i < this->annotations.size(); ++i)
    {
        xml->prependChildElement(this->annotations.getUnchecked(i).serialize());
    }
    
    return xml;
}

void AnnotationEventsGroupInsertAction::deserialize(const XmlElement &xml)
{
    this->reset();
    this->trackId = xml.getStringAttribute(Serialization::Undo::trackId);
    
    forEachXmlChildElement(xml, noteXml)
    {
        AnnotationEvent ae;
        ae.deserialize(*noteXml);
        this->annotations.add(ae);
    }
}

void AnnotationEventsGroupInsertAction::reset()
{
    this->annotations.clear();
    this->trackId.clear();
}


//===----------------------------------------------------------------------===//
// Remove Group
//===----------------------------------------------------------------------===//

AnnotationEventsGroupRemoveAction::AnnotationEventsGroupRemoveAction(ProjectTreeItem &parentProject,
                                                                     String targetTrackId,
                                                                     Array<AnnotationEvent> &target) :
    UndoAction(parentProject),
    trackId(std::move(targetTrackId))
{
    this->annotations.swapWith(target);
}

bool AnnotationEventsGroupRemoveAction::perform()
{
    if (AnnotationsSequence *sequence =
        this->project.findSequenceByTrackId<AnnotationsSequence>(this->trackId))
    {
        return sequence->removeGroup(this->annotations, false);
    }
    
    return false;
}

bool AnnotationEventsGroupRemoveAction::undo()
{
    if (AnnotationsSequence *sequence =
        this->project.findSequenceByTrackId<AnnotationsSequence>(this->trackId))
    {
        return sequence->insertGroup(this->annotations, false);
    }
    
    return false;
}

int AnnotationEventsGroupRemoveAction::getSizeInUnits()
{
    return (sizeof(AnnotationEvent) * this->annotations.size());
}

XmlElement *AnnotationEventsGroupRemoveAction::serialize() const
{
    auto xml = new XmlElement(Serialization::Undo::annotationEventsGroupRemoveAction);
    xml->setAttribute(Serialization::Undo::trackId, this->trackId);
    
    for (int i = 0; i < this->annotations.size(); ++i)
    {
        xml->prependChildElement(this->annotations.getUnchecked(i).serialize());
    }
    
    return xml;
}

void AnnotationEventsGroupRemoveAction::deserialize(const XmlElement &xml)
{
    this->reset();
    this->trackId = xml.getStringAttribute(Serialization::Undo::trackId);
    
    forEachXmlChildElement(xml, noteXml)
    {
        AnnotationEvent ae;
        ae.deserialize(*noteXml);
        this->annotations.add(ae);
    }
}

void AnnotationEventsGroupRemoveAction::reset()
{
    this->annotations.clear();
    this->trackId.clear();
}


//===----------------------------------------------------------------------===//
// Change Group
//===----------------------------------------------------------------------===//

AnnotationEventsGroupChangeAction::AnnotationEventsGroupChangeAction(ProjectTreeItem &parentProject,
                                                                     String targetTrackId,
                                                                     const Array<AnnotationEvent> state1,
                                                                     const Array<AnnotationEvent> state2) :
    UndoAction(parentProject),
    trackId(std::move(targetTrackId))
{
    this->eventsBefore.addArray(state1);
    this->eventsAfter.addArray(state2);
}

bool AnnotationEventsGroupChangeAction::perform()
{
    if (AnnotationsSequence *sequence =
        this->project.findSequenceByTrackId<AnnotationsSequence>(this->trackId))
    {
        return sequence->changeGroup(this->eventsBefore, this->eventsAfter, false);
    }
    
    return false;
}

bool AnnotationEventsGroupChangeAction::undo()
{
    if (AnnotationsSequence *sequence =
        this->project.findSequenceByTrackId<AnnotationsSequence>(this->trackId))
    {
        return sequence->changeGroup(this->eventsAfter, this->eventsBefore, false);
    }
    
    return false;
}

int AnnotationEventsGroupChangeAction::getSizeInUnits()
{
    return (sizeof(AnnotationEvent) * this->eventsBefore.size()) +
        (sizeof(AnnotationEvent) * this->eventsAfter.size());
}

UndoAction *AnnotationEventsGroupChangeAction::createCoalescedAction(UndoAction *nextAction)
{
    if (AnnotationsSequence *sequence =
        this->project.findSequenceByTrackId<AnnotationsSequence>(this->trackId))
    {
        if (AnnotationEventsGroupChangeAction *nextChanger =
            dynamic_cast<AnnotationEventsGroupChangeAction *>(nextAction))
        {
            if (nextChanger->trackId != this->trackId)
            {
                return nullptr;
            }
            
            // simple checking the first and the last ones should be enough here
            bool arraysContainSameEvents =
                (this->eventsBefore.size() == nextChanger->eventsAfter.size()) &&
                (this->eventsBefore[0].getId() == nextChanger->eventsAfter[0].getId());
            
            if (arraysContainSameEvents)
            {
                return new AnnotationEventsGroupChangeAction(this->project,
                    this->trackId, this->eventsBefore, nextChanger->eventsAfter);
            }
        }
    }

    (void) nextAction;
    return nullptr;
}


//===----------------------------------------------------------------------===//
// Serializable
//===----------------------------------------------------------------------===//

XmlElement *AnnotationEventsGroupChangeAction::serialize() const
{
    auto xml = new XmlElement(Serialization::Undo::annotationEventsGroupChangeAction);
    xml->setAttribute(Serialization::Undo::trackId, this->trackId);
    
    auto groupBeforeChild = new XmlElement(Serialization::Undo::groupBefore);
    auto groupAfterChild = new XmlElement(Serialization::Undo::groupAfter);
    
    for (int i = 0; i < this->eventsBefore.size(); ++i)
    {
        groupBeforeChild->prependChildElement(this->eventsBefore.getUnchecked(i).serialize());
    }
    
    for (int i = 0; i < this->eventsAfter.size(); ++i)
    {
        groupAfterChild->prependChildElement(this->eventsAfter.getUnchecked(i).serialize());
    }
    
    xml->prependChildElement(groupBeforeChild);
    xml->prependChildElement(groupAfterChild);
    
    return xml;
}

void AnnotationEventsGroupChangeAction::deserialize(const XmlElement &xml)
{
    this->reset();
    this->trackId = xml.getStringAttribute(Serialization::Undo::trackId);
    
    XmlElement *groupBeforeChild = xml.getChildByName(Serialization::Undo::groupBefore);
    XmlElement *groupAfterChild = xml.getChildByName(Serialization::Undo::groupAfter);
    
    forEachXmlChildElement(*groupBeforeChild, eventXml)
    {
        AnnotationEvent ae;
        ae.deserialize(*eventXml);
        this->eventsBefore.add(ae);
    }
    
    forEachXmlChildElement(*groupAfterChild, eventXml)
    {
        AnnotationEvent ae;
        ae.deserialize(*eventXml);
        this->eventsAfter.add(ae);
    }
}

void AnnotationEventsGroupChangeAction::reset()
{
    this->eventsBefore.clear();
    this->eventsAfter.clear();
    this->trackId.clear();
}
