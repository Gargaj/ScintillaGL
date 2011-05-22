// Scintilla source code edit control
/** @file Style.h
 ** Defines the font and colour style for a class of text.
 **/
// Copyright 1998-2001 by Neil Hodgson <neilh@scintilla.org>
// The License.txt file describes the conditions under which this software may be distributed.

#ifndef STYLE_H
#define STYLE_H

#ifdef SCI_NAMESPACE
namespace Scintilla {
#endif

/**
 */
class Style {
public:
	Colour/*Pair*/ fore;
	Colour/*Pair*/ back;
	bool aliasOfDefaultFont;
	bool bold;
	bool italic;
	int size;
	const char *fontName;
	int characterSet;
	bool eolFilled;
	bool underline;
	enum ecaseForced {caseMixed, caseUpper, caseLower};
	ecaseForced caseForce;
	bool visible;
	bool changeable;
	bool hotspot;

	Font font;
	int sizeZoomed;
	float lineHeight;
	float ascent;
	float descent;
	float externalLeading;
	float aveCharWidth;
	float spaceWidth;

	Style();
	Style(const Style &source);
	~Style();
	Style &operator=(const Style &source);
	void Clear(Colour/*Desired*/ fore_, Colour/*Desired*/ back_,
	           int size_,
	           const char *fontName_, int characterSet_,
	           bool bold_, bool italic_, bool eolFilled_,
	           bool underline_, ecaseForced caseForce_,
		   bool visible_, bool changeable_, bool hotspot_);
	void ClearTo(const Style &source);
	bool EquivalentFontTo(const Style *other) const;
	void Realise(Surface &surface, int zoomLevel, Style *defaultStyle = 0, int extraFontFlag = 0);
	bool IsProtected() const { return !(changeable && visible);}
};

#ifdef SCI_NAMESPACE
}
#endif

#endif
