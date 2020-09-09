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
#include "MeasurementSet.h"

MeasurementSet::MeasurementSet()
{
}

void MeasurementSet::addGroup(CString name, std::vector<CString> m) {
	groups.push_back(name); 
	for (std::vector<CString>::iterator i = m.begin(); i != m.end(); i++) {
		addMeasurement(*i, name); 
	}
}

void MeasurementSet::addMeasurement(CString measurementName, CString groupName) {
	int index = measurementID(measurementName);
	if(index == -1)
	{
		measurements.push_back(measurementName); 
		measurementToGroup.push_back(groupID(groupName));
	}
}

void MeasurementSet::modifyMeasurement(CString oldName, CString newName)
{
	int index = measurementID(oldName);
	if(index != -1)
		measurements[index] = newName;
}

void MeasurementSet::removeMeasurement(CString measurementName)
{
	int index = measurementID(measurementName);
	if(index != -1)
	{
		if(measurements[index] == measurements.back())
		{
			measurements.pop_back();
		}
		else
		{
			measurements.erase(measurements.begin()+index);
		}
	}
}

void MeasurementSet::clearGroup(CString groupName) {
	int groupIndex = groupID(groupName); 
	if (groupIndex != -1) 
	{ 
		int start, end; 
		for (start = 0; start < measurements.size() && groupForMeasurement(start) != groupIndex; start++) { }
		for (end = start; end < measurements.size() && groupForMeasurement(end) == groupIndex; end++) { }
		measurements.erase(measurements.begin() + start, measurements.begin() + end); 
		measurementToGroup.erase(measurementToGroup.begin() + start, measurementToGroup.begin() + end); 
	}
}

void MeasurementSet::removeGroup(CString groupName) {
	int groupIndex = groupID(groupName); 
	if (groupIndex != -1) 
	{ 
		clearGroup(groupName); 
		groups.erase(groups.begin() + groupIndex); 
		for (int i = 0; i < measurementToGroup.size(); i++) {
			if (measurementToGroup[i] > groupIndex) {
				measurementToGroup[i]--; 
			}
		}
	}
}

int MeasurementSet::groupID(CString groupName) const {
	return indexOf(groups, groupName); 
}

int MeasurementSet::measurementID(CString measurementName) const {
	return indexOf(measurements, measurementName); 
}

int MeasurementSet::indexOf(const std::vector<CString>& v, const CString& value) {
	for (int i = 0; i < v.size(); i++) {
		if (value == v[i]) {
			return i; 
		}
	}
	return -1; 
}

CString MeasurementSet::groupName(int groupID) const {
	if (0 <= groupID && groupID < groups.size()) 
		return groups[groupID]; 
	return _T(""); 
}

CString MeasurementSet::measurementName(int measurementID) const {
	if (0 <= measurementID && measurementID < measurements.size()) 
		return measurements[measurementID]; 
	return _T(""); 
}

int MeasurementSet::numGroups() const {
	return groups.size(); 
}

int MeasurementSet::numMeasurements() const {
	return measurements.size(); 
}

int MeasurementSet::groupForMeasurement(int measurementID) const {
	if (0 <= measurementID && measurementID < measurementToGroup.size()) 
		return measurementToGroup[measurementID]; 
	return -1; 
}

std::vector<CString> MeasurementSet::measurementsInGroup(CString group) const {
	std::vector<CString> ret; 
	int id = groupID(group); 
	for (int i = 0; i < measurementToGroup.size(); i++) {
		if (measurementToGroup[i] == id) {
			ret.push_back(measurementName(i)); 
		}
	}
	return ret; 
}
