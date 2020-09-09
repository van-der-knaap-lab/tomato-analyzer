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

#include <vector>
#include <map>


/**
 * A MeasurementSet represents a set of fruit measurements arranged in groups. 
 * Each measurement belongs to exactly one group. Each measurement and group has a name and a unique ID number. 
 * The MeasurementSet does not store any data for the measurements, or whether or not
 * a measurement has been selected by the user to view/export -- just names and IDs. 
 * 
 * The group and measurement IDs are numbered ascendingly and consecutively in the order 
 * that groups and measurements are added. IDs are adjusted to maintain this invariant 
 * when measurements are removed. 
 * 
 * All groups and measurements are assumed to have unique names. There can't be two 
 * measurements with the same name in different groups. This makes sense because when the data
 * is exported to a CSV file all column names should be unique. 
 * 
 * Measurement names should not contain commas, since they need to be exported to a CSV file. 
 */
class MeasurementSet
{
public:
	friend class CTomatoAnalyzerApp;

	/**
	 * Constructs an empty measurement set. 
	 */
	MeasurementSet(); 

	/**
	 * Adds a group of measurements, using the given name for the group and the given 
	 * vector of names for the measurements. 
	 */
	void addGroup(CString name, std::vector<CString> measurements); 

	/**
	 * Adds a measurement with the given name to the group with the given name. 
	 */
	void addMeasurement(CString measurementName, CString groupName); 

	/**
	 * Changes the name of a measurement from oldName to newName. Does nothing if there is no measurement by that name. 
	 */ 
	void modifyMeasurement(CString oldName, CString newName);

	/**
	 * Removes the measurement from the measurement set, adjusting the IDs of all measurements with higher IDs. 
	 * Does not remove the measurement's group. 
	 */
	void removeMeasurement(CString measurementName);

	/**
	 * Removes all measurements from the group, adjusting the IDs of all measurements with higher IDs. 
	 * Does not remove the group. 
	 */
	void clearGroup(CString groupName); 

	/**
	 * Removes the group from the measurement set, adjusting the IDs of all groups and measurements with higher IDs. 
	 */
	void removeGroup(CString groupName); 

	/**
	 * Returns the ID for the group with the given name, or -1 if the group does not exist.  
	 */ 
	int groupID(CString groupName) const; 

	/** 
	 * Returns the ID for the measurement with the given name, or -1 if the measurement does not exist. 
	 */
	int measurementID(CString measurementName) const; 

	/**
	 * Returns the name for the group with the given ID, or an empty CString if the group does not exist. 
	 */
	CString groupName(int groupID) const; 

	/**
	 * Returns the name for the measurement with the given ID, or an empty CString if the measurement does not exist. 
	 */
	CString measurementName(int measurementID) const; 

	/**
	 * Returns the number of measurement groups in this measurement set. 
	 */
	int numGroups() const; 

	/**
	 * Returns the number of measurements in this measurement set. 
	 */
	int numMeasurements() const; 

	/**
	 * Returns the ID of the group to which the measurement with the given ID belongs, or -1 if the measurement
	 * does not exist. 
	 */
	int groupForMeasurement(int measurementID) const; 

	/**
	 * Returns a list of measurement names belonging to the given group name, in ascending order of ID. 
	 */
	std::vector<CString> measurementsInGroup(CString group) const; 

private: 
	static int indexOf(const std::vector<CString>& v, const CString& value); 

	/// The list of groups, in the order added. 
	std::vector<CString> groups;  

	/// The list of measurements, in the order added. 
	std::vector<CString> measurements; 

	/// A mapping from measurement IDs to parent group IDs. 
	std::vector<int> measurementToGroup; 
};

