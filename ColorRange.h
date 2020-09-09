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
#pragma once

/**
 * A color can be uniquely defined by its L*, hue, and chroma. 
 * A ColorRange object represents a range of L*, hue, and chroma values, into which specific colors may fall. 
 */ 
class ColorRange { 

private: 
	int lowerL, upperL, lowerHue, upperHue, lowerChroma, upperChroma; 
	bool emptyL, emptyHue, emptyChroma; 

public: 
	/**
	 * Creates a range with L*, hue, and chroma all as wildcards. Any color falls into this range. 
	 */
	ColorRange(); 

	/**
	 * Creates a range such that each parameter (X) is either: 
	 * - if wildcardX is true, a wildcard parameter representing all possible values of X
	 * - otherwise, the range [lowerX, upperX)
	 * 
	 * lowerHue and upperHue are transformed to a value in [0, 360). 
	 */
	ColorRange(int lowerL, int upperL, int lowerHue, int upperHue, int lowerChroma, int upperChroma, 
		bool wildcardL, bool wildcardHue, bool wildcardChroma);

	/** 
	 * Returns true if the given L*, hue, and chroma all fall within this range. 
	 * 
	 * Since hue is an angle on a circle, it is treated as circular. For example: 
	 * - 30 is in the range [0, 40) but not [40, 0)
	 * - 355 and 5 are in the range [330, 30)
	 */
	bool isInRange(double l, double hue, double chroma); 

	int getLowerL() { return lowerL; }
	int getUpperL() { return upperL; }
	int getLowerHue() { return lowerHue; }
	int getUpperHue() { return upperHue; }
	int getLowerChroma() { return lowerChroma; }
	int getUpperChroma() { return upperChroma; }
	bool isWildcardL() { return emptyL; }
	bool isWildcardHue() { return emptyHue; }
	bool isWildcardChroma() { return emptyChroma; }

	CString toString(); 
	CString toStringL(); 
	CString toStringHue(); 
	CString toStringChroma(); 
}; 

