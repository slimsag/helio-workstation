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

//[Headers]
#include "Common.h"
//[/Headers]

#include "ChordTooltip.h"

//[MiscUserDefs]
//[/MiscUserDefs]

ChordTooltip::ChordTooltip(String rootKey, String scale, String function)
{
    addAndMakeVisible (rootKeyLabel = new Label (String(),
                                                 TRANS("popup::chord::rootkey")));
    rootKeyLabel->setFont (Font (Font::getDefaultSerifFontName(), 16.00f, Font::plain).withTypefaceStyle ("Regular"));
    rootKeyLabel->setJustificationType (Justification::centredRight);
    rootKeyLabel->setEditable (false, false, false);
    rootKeyLabel->setColour (Label::textColourId, Colour (0xa0ffffff));
    rootKeyLabel->setColour (TextEditor::textColourId, Colours::black);
    rootKeyLabel->setColour (TextEditor::backgroundColourId, Colour (0x00000000));

    addAndMakeVisible (functionLabel = new Label (String(),
                                                  TRANS("popup::chord::function")));
    functionLabel->setFont (Font (Font::getDefaultSerifFontName(), 16.00f, Font::plain).withTypefaceStyle ("Regular"));
    functionLabel->setJustificationType (Justification::centredRight);
    functionLabel->setEditable (false, false, false);
    functionLabel->setColour (Label::textColourId, Colour (0xa0ffffff));
    functionLabel->setColour (TextEditor::textColourId, Colours::black);
    functionLabel->setColour (TextEditor::backgroundColourId, Colour (0x00000000));

    addAndMakeVisible (scaleLabel = new Label (String(),
                                               TRANS("popup::chord::scale")));
    scaleLabel->setFont (Font (Font::getDefaultSerifFontName(), 16.00f, Font::plain).withTypefaceStyle ("Regular"));
    scaleLabel->setJustificationType (Justification::centredRight);
    scaleLabel->setEditable (false, false, false);
    scaleLabel->setColour (Label::textColourId, Colour (0xa0ffffff));
    scaleLabel->setColour (TextEditor::textColourId, Colours::black);
    scaleLabel->setColour (TextEditor::backgroundColourId, Colour (0x00000000));

    addAndMakeVisible (rooKeyValue = new Label (String(),
                                                TRANS("...")));
    rooKeyValue->setFont (Font (Font::getDefaultSerifFontName(), 21.00f, Font::plain).withTypefaceStyle ("Regular"));
    rooKeyValue->setJustificationType (Justification::centredLeft);
    rooKeyValue->setEditable (false, false, false);
    rooKeyValue->setColour (Label::textColourId, Colours::white);
    rooKeyValue->setColour (TextEditor::textColourId, Colours::black);
    rooKeyValue->setColour (TextEditor::backgroundColourId, Colour (0x00000000));

    addAndMakeVisible (functionValue = new Label (String(),
                                                  TRANS("...")));
    functionValue->setFont (Font (Font::getDefaultSerifFontName(), 21.00f, Font::plain).withTypefaceStyle ("Regular"));
    functionValue->setJustificationType (Justification::centredLeft);
    functionValue->setEditable (false, false, false);
    functionValue->setColour (Label::textColourId, Colours::white);
    functionValue->setColour (TextEditor::textColourId, Colours::black);
    functionValue->setColour (TextEditor::backgroundColourId, Colour (0x00000000));

    addAndMakeVisible (scaleValue = new Label (String(),
                                               TRANS("...")));
    scaleValue->setFont (Font (Font::getDefaultSerifFontName(), 21.00f, Font::plain).withTypefaceStyle ("Regular"));
    scaleValue->setJustificationType (Justification::centredLeft);
    scaleValue->setEditable (false, false, false);
    scaleValue->setColour (Label::textColourId, Colours::white);
    scaleValue->setColour (TextEditor::textColourId, Colours::black);
    scaleValue->setColour (TextEditor::backgroundColourId, Colour (0x00000000));


    //[UserPreSize]
    scaleValue->setText(scale, dontSendNotification);
    rooKeyValue->setText(rootKey, dontSendNotification);
    functionValue->setText(function, dontSendNotification);
    //[/UserPreSize]

    setSize (500, 80);

    //[Constructor]
    //[/Constructor]
}

ChordTooltip::~ChordTooltip()
{
    //[Destructor_pre]
    //[/Destructor_pre]

    rootKeyLabel = nullptr;
    functionLabel = nullptr;
    scaleLabel = nullptr;
    rooKeyValue = nullptr;
    functionValue = nullptr;
    scaleValue = nullptr;

    //[Destructor]
    //[/Destructor]
}

void ChordTooltip::paint (Graphics& g)
{
    //[UserPrePaint] Add your own custom painting code here..
    //[/UserPrePaint]

    //[UserPaint] Add your own custom painting code here..
    //[/UserPaint]
}

void ChordTooltip::resized()
{
    //[UserPreResize] Add your own custom resize code here..
    //[/UserPreResize]

    rootKeyLabel->setBounds ((getWidth() / 2) + -98 - 128, (getHeight() / 2) + -24 - (22 / 2), 128, 22);
    functionLabel->setBounds ((getWidth() / 2) + -98 - 128, (getHeight() / 2) + 24 - (22 / 2), 128, 22);
    scaleLabel->setBounds ((getWidth() / 2) + -98 - 128, (getHeight() / 2) - (22 / 2), 128, 22);
    rooKeyValue->setBounds ((getWidth() / 2) + -98, (getHeight() / 2) + -24 - (22 / 2), 340, 22);
    functionValue->setBounds ((getWidth() / 2) + -98, (getHeight() / 2) + 24 - (22 / 2), 340, 22);
    scaleValue->setBounds ((getWidth() / 2) + -98, (getHeight() / 2) - (22 / 2), 340, 22);
    //[UserResized] Add your own custom resize handling here..
    //[/UserResized]
}


//[MiscUserCode]
//[/MiscUserCode]

#if 0
/*
BEGIN_JUCER_METADATA

<JUCER_COMPONENT documentType="Component" className="ChordTooltip" template="../../../Template"
                 componentName="" parentClasses="public Component" constructorParams="String rootKey, String scale, String function"
                 variableInitialisers="" snapPixels="4" snapActive="1" snapShown="1"
                 overlayOpacity="0.330" fixedSize="1" initialWidth="500" initialHeight="80">
  <BACKGROUND backgroundColour="ffffff"/>
  <LABEL name="" id="15f9a8211950d493" memberName="rootKeyLabel" virtualName=""
         explicitFocusOrder="0" pos="-98Cr -24Cc 128 22" textCol="a0ffffff"
         edTextCol="ff000000" edBkgCol="0" labelText="popup::chord::rootkey"
         editableSingleClick="0" editableDoubleClick="0" focusDiscardsChanges="0"
         fontname="Default serif font" fontsize="16" kerning="0" bold="0"
         italic="0" justification="34"/>
  <LABEL name="" id="a099f8af50cb617c" memberName="functionLabel" virtualName=""
         explicitFocusOrder="0" pos="-98Cr 24Cc 128 22" textCol="a0ffffff"
         edTextCol="ff000000" edBkgCol="0" labelText="popup::chord::function"
         editableSingleClick="0" editableDoubleClick="0" focusDiscardsChanges="0"
         fontname="Default serif font" fontsize="16" kerning="0" bold="0"
         italic="0" justification="34"/>
  <LABEL name="" id="cfca17160c720838" memberName="scaleLabel" virtualName=""
         explicitFocusOrder="0" pos="-98Cr 0Cc 128 22" textCol="a0ffffff"
         edTextCol="ff000000" edBkgCol="0" labelText="popup::chord::scale"
         editableSingleClick="0" editableDoubleClick="0" focusDiscardsChanges="0"
         fontname="Default serif font" fontsize="16" kerning="0" bold="0"
         italic="0" justification="34"/>
  <LABEL name="" id="9aa11c8ad14cfb53" memberName="rooKeyValue" virtualName=""
         explicitFocusOrder="0" pos="-98C -24Cc 340 22" textCol="ffffffff"
         edTextCol="ff000000" edBkgCol="0" labelText="..." editableSingleClick="0"
         editableDoubleClick="0" focusDiscardsChanges="0" fontname="Default serif font"
         fontsize="21" kerning="0" bold="0" italic="0" justification="33"/>
  <LABEL name="" id="cd15c9237a49389c" memberName="functionValue" virtualName=""
         explicitFocusOrder="0" pos="-98C 24Cc 340 22" textCol="ffffffff"
         edTextCol="ff000000" edBkgCol="0" labelText="..." editableSingleClick="0"
         editableDoubleClick="0" focusDiscardsChanges="0" fontname="Default serif font"
         fontsize="21" kerning="0" bold="0" italic="0" justification="33"/>
  <LABEL name="" id="3354bc8b8e7454a6" memberName="scaleValue" virtualName=""
         explicitFocusOrder="0" pos="-98C 0Cc 340 22" textCol="ffffffff"
         edTextCol="ff000000" edBkgCol="0" labelText="..." editableSingleClick="0"
         editableDoubleClick="0" focusDiscardsChanges="0" fontname="Default serif font"
         fontsize="21" kerning="0" bold="0" italic="0" justification="33"/>
</JUCER_COMPONENT>

END_JUCER_METADATA
*/
#endif
