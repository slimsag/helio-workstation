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
#include "Pattern.h"
#include "PatternActions.h"
#include "ProjectTreeItem.h"
#include "ProjectEventDispatcher.h"
#include "UndoStack.h"
#include "SerializationKeys.h"
#include "MidiTrack.h"

Pattern::Pattern(MidiTrack &parentTrack,
    ProjectEventDispatcher &dispatcher) :
    track(parentTrack),
    eventDispatcher(dispatcher)
{
    // Add default single instance?
    this->clips.add(Clip(this));
}

Pattern::~Pattern()
{
    this->masterReference.clear();
}

void Pattern::sort()
{
    if (this->clips.size() > 0)
    {
        const Clip clip(this);
        this->clips.sort(clip);
    }
}

//===----------------------------------------------------------------------===//
// Undoing // TODO move this to project interface
//===----------------------------------------------------------------------===//

void Pattern::checkpoint()
{
    this->getUndoStack()->beginNewTransaction(String::empty);
}

void Pattern::undo()
{
    if (this->getUndoStack()->canUndo())
    {
        this->checkpoint();
        this->getUndoStack()->undo();
    }
}

void Pattern::redo()
{
    if (this->getUndoStack()->canRedo())
    {
        this->getUndoStack()->redo();
    }
}

void Pattern::clearUndoHistory()
{
    this->getUndoStack()->clearUndoHistory();
}

//===----------------------------------------------------------------------===//
// Clip Actions
//===----------------------------------------------------------------------===//

Array<Clip> &Pattern::getClips() noexcept
{
    return this->clips;
}

void Pattern::silentImport(const Clip &clip)
{
    if (! this->clips.contains(clip))
    {
        this->clips.addSorted(clip, clip);
    }
}

bool Pattern::insert(Clip clip, const bool undoable)
{
    if (this->clips.contains(clip))
    {
        return false;
    }

    if (undoable)
    {
        this->getUndoStack()->perform(new PatternClipInsertAction(*this->getProject(),
            this->getTrackId(),
            clip));
    }
    else
    {
        this->clips.addSorted(clip, clip);
        this->notifyClipAdded(clip);
    }

    return true;
}

bool Pattern::remove(Clip clip, const bool undoable)
{
    if (undoable)
    {
        this->getUndoStack()->perform(new PatternClipRemoveAction(*this->getProject(),
            this->getTrackId(),
            clip));
    }
    else
    {
        int index = this->clips.indexOfSorted(clip, clip);
        if (index >= 0)
        {
            this->clips.remove(index);
            this->notifyClipRemoved(clip);
            return true;
        }

        return false;
    }

    return true;
}

bool Pattern::change(Clip clip, Clip newClip, const bool undoable)
{
    if (undoable)
    {
        this->getUndoStack()->perform(new PatternClipChangeAction(*this->getProject(),
            this->getTrackId(),
            clip,
            newClip));
    }
    else
    {
        int index = this->clips.indexOfSorted(clip, clip);
        if (index >= 0)
        {
            this->clips.remove(index);
            this->clips.addSorted(newClip, newClip);
            this->notifyClipChanged(clip, newClip);
            return true;
        }

        return false;
    }

    return true;
}


//===----------------------------------------------------------------------===//
// Accessors
//===----------------------------------------------------------------------===//

ProjectTreeItem *Pattern::getProject()
{
    return this->eventDispatcher.getProject();
}

UndoStack *Pattern::getUndoStack()
{
    return this->eventDispatcher.getProject()->getUndoStack();
}

MidiTrack *Pattern::getTrack() const noexcept
{
    return &this->track;
}


//===----------------------------------------------------------------------===//
// Events change listener
//===----------------------------------------------------------------------===//

void Pattern::notifyClipChanged(const Clip &oldClip, const Clip &newClip)
{
    this->eventDispatcher.dispatchChangeClip(oldClip, newClip);
}

void Pattern::notifyClipAdded(const Clip &clip)
{
    this->eventDispatcher.dispatchAddClip(clip);
}

void Pattern::notifyClipRemoved(const Clip &clip)
{
    this->eventDispatcher.dispatchRemoveClip(clip);
}

void Pattern::notifyClipRemovedPostAction()
{
    this->eventDispatcher.dispatchPostRemoveClip(this);
}

void Pattern::notifyPatternChanged()
{
    this->eventDispatcher.dispatchChangeTrackContent(&this->track);
}


//===----------------------------------------------------------------------===//
// Serializable
//===----------------------------------------------------------------------===//

XmlElement *Pattern::serialize() const
{
    auto xml = new XmlElement(Serialization::Core::pattern);

    for (int i = 0; i < this->clips.size(); ++i)
    {
        const Clip clip = this->clips.getUnchecked(i);
        xml->prependChildElement(clip.serialize());
    }

    return xml;
}

void Pattern::deserialize(const XmlElement &xml)
{
    this->clearQuick();

    const XmlElement *root =
        (xml.getTagName() == Serialization::Core::pattern) ?
        &xml : xml.getChildByName(Serialization::Core::pattern);

    if (root == nullptr)
    {
        return;
    }

    forEachXmlChildElementWithTagName(*root, e, Serialization::Core::clip)
    {
        Clip c(this);
        c.deserialize(*e);
        this->clips.add(c);
    }

    // Fallback to single clip at zero bar, if no clips found
    if (this->clips.size() == 0)
    {
        this->clips.add(Clip(this));
    }

    this->sort();
    this->notifyPatternChanged();
}

void Pattern::reset()
{
    this->clearQuick();
    this->notifyPatternChanged();
}

void Pattern::clearQuick()
{
    this->clips.clearQuick();
}


//===----------------------------------------------------------------------===//
// Helpers
//===----------------------------------------------------------------------===//

String Pattern::getTrackId() const noexcept
{
    return this->track.getTrackId().toString();
}

int Pattern::hashCode() const noexcept
{
    return this->getTrackId().hashCode();
}
