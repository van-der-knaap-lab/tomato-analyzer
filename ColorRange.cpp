/* This file is part of Tomato Analyzer.

    Tomato Analyzer is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Tomato Analyzer is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Tomato Analyzer.  If not, see <http://www.gnu.org/licenses/>. */
#include "stdafx.h"
#include "resource.h"
#include <string>
#include "ColorRange.h"
#include "Msg.h"
using namespace std; 

ColorRange::ColorRange() {
	emptyL = emptyHue = emptyChroma = true; 
}

ColorRange::ColorRange(int ll, int ul, int lh, int uh, int lc, int uc, bool el, bool eh, bool ec) {
	lowerL = ll; upperL = ul; lowerHue = lh; upperHue = uh; lowerChroma = lc; upperChroma = uc; 
	emptyL = el; emptyHue = eh; emptyChroma = ec; 
	while (lowerHue < 0)  lowerHue += 360; 
	while (upperHue < 0)  upperHue += 360; 
	while (lowerHue >= 360)  lowerHue -= 360; 
	while (upperHue >= 360)  upperHue -= 360; 
}

bool ColorRange::isInRange(double l, double hue, double chroma) {
	return ((emptyL || (lowerL <= l && l < upperL)) && 
		(emptyHue || (lowerHue <= upperHue ? (lowerHue <= hue && hue < upperHue) : 
											 (lowerHue <= hue || (0 <= hue && hue < upperHue)))) && 
		(emptyChroma || (lowerChroma <= chroma && chroma < upperChroma))); 
}

CString ColorRange::toString() {
	CString s; 
	s.Format(_T("%s %s %s"), toStringL(), toStringHue(), toStringChroma()); 
	return s; 
}

CString ColorRange::toStringL() {
	CString s; 
	if (emptyL) {
		s.Format(_T("L[)")); 
	} else {
		s.Format(_T("L[%d..%d)"), lowerL, upperL); 
	}
	return s; 
}

CString ColorRange::toStringHue() {
	CString s; 
	if (emptyHue) {
		s.Format(_T("H[)")); 
	} else {
		s.Format(_T("H[%d..%d)"), lowerHue, upperHue); 
	}
	return s; 
}

CString ColorRange::toStringChroma() {
	CString s; 
	if (emptyChroma) {
		s.Format(_T("C[)")); 
	} else {
		s.Format(_T("C[%d..%d)"), lowerChroma, upperChroma); 
	}
	return s; 
}
