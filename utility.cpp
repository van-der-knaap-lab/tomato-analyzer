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

#include <limits.h>
#include <vector>
#include <string>

using namespace std;

/**
 * statically declared variables, used locally
 */
static const char * szDelim = ", \t\n\r";
static const char * szWhiteSpace = " \t\n\r";
static const char * szNumeric = "0123456789+-.eE";

/**
 * statically declared functions, used locally
 */
static bool isDelim(const char c);
static int toIntAFAP(const string& str, string::size_type& startpos);
static int getNumericalCharPosition(const string& str, 
                                    bool& bpositive, 
                                    string::size_type& startpos,  
                                    string::size_type& endpos);
static int atof(const string& str, 
                double& dat, 
                bool& bpositive, 
                string::size_type& startpos);

/**
 * convert string into integer
 * unlike the other function atoi(const char *), it can indicate if the
 * conversion is successful or not.
 * Another difference is, the input string can contain 0-9, +, -, and space only.
 *
 * NOTICE:
 * IF THE INPUT STRING IS EMPTY, RETURN TRUE WITH CONVERTED INTEGER OF ZERO
 *
 */
bool atoi(const string& str, int& res)
{
    string::size_type   pos = str.find_first_not_of(' ', 0);
    if(string::npos == pos)
    {
        res = 0;
        return true;
    }

    bool positive = true;
    char chTmp = str[pos];
    if('-' == chTmp)
    {
        positive = false;
        ++pos;
    }
    else if('+' == chTmp)
    {
        positive = true;
        ++pos;
    }
    
    string::size_type   len = str.length();
    int cvtResult = 0;
    while(pos < len)
    {
        chTmp = str[pos];
        if(chTmp<'0' || chTmp>'9')
            return false;

        cvtResult = cvtResult*10 + chTmp - '0';

        ++pos;
    }

    res = (positive)? cvtResult: -cvtResult;
    return true;
}


/**
 * extract integer
 * return -1, conversion fail
 *         0, nothing to convert
 *         1, conversion successful
 */
int atonum(const string& str, int& res, string::size_type& startpos)
{
    res = 0;

    bool bpositive;
    string::size_type  endpos;
    int result = getNumericalCharPosition(str, bpositive, startpos, endpos);
    if(1 != result)
        return result;

    int dat = toIntAFAP(str, startpos);
    if(-1==dat || startpos!=endpos)
        return -1;

    res = (bpositive)? dat : -dat;
    return 1;
}


int atonum(const string& str, unsigned char & res, string::size_type& startpos)
{
    int result, val;
    result = atonum(str, val, startpos);

    if(1 == result)
    {
        if(val<0 || val>UCHAR_MAX)
            result = -1;
        else
            res = static_cast<unsigned char>(val);
    }

    return result;
}


int atonum(const string& str, char & res, string::size_type& startpos)
{
    int result, val;
    result = atonum(str, val, startpos);

    if(1 == result)
    {
        if(val<CHAR_MIN || val>CHAR_MAX)
            result = -1;
        else
            res = static_cast<unsigned char>(val);
    }

    return result;
}


int atonum(const string& str, double& dat, string::size_type& startpos)
{
    bool bpositive;
    int result = atof(str, dat, bpositive, startpos);
    if(1==result && false==bpositive)
        dat = -dat;

    return result;
}


int atonum(const string& str, float& dat, string::size_type& startpos)
{
    double val;
    int    result;

    result = atonum(str, val, startpos);
    if(1 == result)
        dat = static_cast<float>(val);

    return result;
}


void replace(string& str, char replacee, char replacer)
{
    string::iterator    iter = str.begin();
    string::iterator    iter_end = str.end();
    while(iter != iter_end)
    {
        if(replacee == *iter)
        {
            *iter = replacer;
        }

        ++iter;
    }

    return;
}


void trim_left(string& str, char chtrm)
{
    string::size_type  pos = str.find_first_not_of(chtrm, 0);
    if(string::npos == pos)
        return;

    string  result(str, pos, (str.length()-pos+1));
    str = result;
    return;
}


void trim_right(string& src, char chtrm)
{
    string::size_type  pos = src.find_last_not_of(chtrm);
    if(string::npos == pos)
        return;

    string  result(src, 0, pos+1);
    src = result;

    return;
}

void trim_bothends(string& src, char chtrm)
{
    trim_left(src, chtrm);
    trim_right(src, chtrm);
    return;
}

bool  getNextline(string& str, const char * szBuffer, int& startpos)
{
    int nlength = static_cast<int>(strlen(szBuffer));
    if(startpos>=nlength)
        return false;

    int offpos = startpos;
    while(startpos<nlength)
    {
        if('\n'==szBuffer[startpos])
        {
            ++startpos;
            if(startpos<nlength)
            {
                if('\r' == szBuffer[startpos])
                {
                    ++startpos;
                    break;
                }
            }
        }

        ++startpos;
    }

    str.assign(szBuffer, offpos, startpos-offpos);
    return true;
}

bool findCommaSeperatedAttributePair(const string& strSrc,
                         string& attr,
                         string& val,
                         string::size_type& startPos)
{
    if(string::npos == startPos)
        return false;
    
    ++startPos;

    string::size_type   len = strSrc.length();
    string::size_type   attrEndPos = strSrc.find(',', startPos);
    if(string::npos == attrEndPos)
        attrEndPos = len - 1;

    string::size_type  valStartPos = strSrc.find_first_of('(', startPos);
    if(string::npos == valStartPos)
        return false;

    string::size_type  valEndPos = strSrc.find_last_of(')', attrEndPos);
    if(string::npos == valStartPos)
        return false;

    ++valStartPos;
    if(valStartPos>valEndPos)
        return false;
    
    attr.assign(strSrc, startPos, valStartPos - 1 - startPos);
    trim_bothends(attr, ' ');

    val.assign(strSrc, valStartPos, valEndPos - valStartPos);
    trim_bothends(val, ' ');

    if((attrEndPos+1) == len)
        startPos = string::npos;
    else
        startPos = attrEndPos;

    return true;
}




/***********************************************************************************/
/*                           private functions                                     */
/***********************************************************************************/
int getNumericalCharPosition(const string& str, 
                             bool& bpositive, 
                             string::size_type& startpos,  
                             string::size_type& endpos)
{

    //beginning position     
    startpos = str.find_first_not_of(szWhiteSpace, startpos);
    if(string::npos == startpos)
        return 0;

    string::size_type  len = str.length();

    if(',' == str[startpos])
    {
        startpos++;
        if(startpos >= len)
            return 0;

        startpos = str.find_first_not_of(szWhiteSpace, startpos);
        if(string::npos == startpos)
            return 0;
        
        if(',' == str[startpos])
            return -1;
    }

    if('+' == str[startpos])
    {
        bpositive = true;

        startpos++;
        if(startpos >= len)
            return -1;
    }
    else if('-' == str[startpos])
    {
        bpositive = false;

        startpos++;
        if(startpos >= len)
            return -1;
    }
    
    //end position
    if(startpos+1 == len)
        endpos = len;
    else
    {
        endpos = str.find_first_not_of(szNumeric, startpos+1);
        if(string::npos == endpos)
            endpos = len;
        else if(!isDelim(str[endpos]))
            return -1;
    }
    return 1;
}


bool isDelim(const char c)
{
    if(' ' == c)
        return true;

    if('\t' == c)
        return true;

    if('\r'==c || '\n'==c || ','==c)
        return true;

    return false;
}


int toIntAFAP(const string& str, string::size_type& startpos)
{
    if(str[startpos]<='9' && str[startpos]>='0')
    {
        int ret = str[startpos] - '0';
        string::size_type   len = str.length();

        while(++startpos < len)
        {
            int tmp = str[startpos] - '0';
            if(tmp>9 || tmp<0)
                break;

            ret = ret*10 + tmp;            
        }

        return ret;
    }

    return -1;
}

int atof(const string& str, double& dat, bool& bpositive, string::size_type& startpos)
{

    bpositive = true;
    string::size_type  endpos;
    int result = getNumericalCharPosition(str, bpositive, startpos, endpos);
    if(1 != result)
        return result;

    bool preexp = false;

    dat = 0.0;
    if(str[startpos]<='9' && str[startpos]>='0')
    {
        preexp = true;

        int tmp = toIntAFAP(str, startpos);
        dat = static_cast<double>(tmp);

        if(startpos == endpos)
            return 1;            
    }

    if('.' == str[startpos])
    {
        startpos++;
        if(startpos==endpos)
        {
            if(false==preexp)
                return -1;

            return 1;
        }

        if(str[startpos]<='9' && str[startpos]>='0')
        {
            preexp = true;
            string::size_type  poswas = startpos;
            
            int dgt = toIntAFAP(str, startpos);
            int dgtNum = static_cast<int>(startpos - poswas);
            double fra = static_cast<double>(dgt);
            while(--dgtNum >= 0)
                fra /= 10.0;

            dat += fra;

            if(startpos == endpos)
                return 1;            
        }
        else if(false == preexp)
        {
            return -1;
        }
    }

    if('e'==str[startpos] || 'E'==str[startpos])
    {
        startpos++;
        if(startpos == endpos)
        {
            if(false == preexp)
                return -1;

            return 1;
        }

        if(false == preexp)
            dat = 1.0;

        bool bexpsign = true;
        if('+' == str[startpos])
        {
            startpos++;
            if(startpos == endpos)
                return -1;
        }
        else if('-' == str[startpos])
        {
            startpos++;
            if(startpos == endpos)
                return -1;

            bexpsign = false;
        }

        int expPart = toIntAFAP(str, startpos);
        if(startpos != endpos)
            return -1;

        if(-1 != expPart)
        {
            if(bexpsign)
            {
                while(--expPart >= 0)
                    dat *= 10.0;
            }
            else
            {
                while(--expPart >= 0)
                    dat /= 10.0;
            }

            return 1;
        }
    }

    return -1;
}


int atovct(const string& str, vector<int>& vct)
{
    vct.clear();

    int dat;
    string::size_type   pos = 0;
    int result = atonum(str, dat, pos);
    while(1 == result)
    {
        vct.push_back(dat);
        result = atonum(str, dat, pos);
    }
    
    if(vct.size()>0 && 0==result)
        result = 1;

    return result;
}


int atovct(const string& str, vector<float>& vct)
{
    vct.clear();

    float dat;
    string::size_type   pos = 0;
    int result = atonum(str, dat, pos);
    while(1 == result)
    {
        vct.push_back(dat);
        result = atonum(str, dat, pos);
    }
    
    if(vct.size()>0 && 0==result)
        result = 1;

    return result;
}


void sortvalue(std::vector<int>& row, std::vector<int>& seqIndex)
{
    int nCount = row.size();

    for(int i=0; i<nCount-1; i++)
    {
        int prev = row[i];

        for(int j=i+1; j<nCount; j++)
        {
            int cur, tmpSeq;

            cur = row[j];
            if(prev>cur)
            {
                row[i] = cur;
                row[j] = prev;
                prev = cur;

                tmpSeq = seqIndex[i];
                seqIndex[i] = seqIndex[j];
                seqIndex[j] = tmpSeq;
            }
        }
    }
    return;
}

