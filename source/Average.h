#ifndef _Average_H_INCLUDED_
#define _Average_H_INCLUDED_

#define ARBITRARY_AVG_VAL -1

#include <vector>
#include <assert.h>


template <class TavgVal>

class Average
{
public:
   //////////////////////////////////////////////////////////////////////////
   // Construction / Destruction
/*********************************************************************************************
 Class      : Average
 Function   : Average()
 Description: Creates an array to store values of arbitrary types.
              Sample: Average<double> ArrayWithFloatingPointValues

 Parameters: None

 Result type:  ()
 created: June, 16 2005
 <TITLE Average>
*********************************************************************************************/
   Average()
   {
      m_nNoOfAvgValues = ARBITRARY_AVG_VAL;
      m_nCurrentValue  = 0;
      m_nCurrenSize     = 0;
      m_AvgSum           = (TavgVal)0;
   }
   //////////////////////////////////////////////////////////////////////////
   ~Average()
   {
      clear();
   }

   //////////////////////////////////////////////////////////////////////////
   // Implementation
/*********************************************************************************************
 Class      : Average
 Function   : void removeAll()
 Description: Frees all Data

 Parameters: None

 Result type:  (void)
 created: June, 16 2005
 <TITLE removeAll>
*********************************************************************************************/
   void clear()
   {
      m_AvgValues.clear();
      m_nCurrenSize   = 0;
      m_nCurrentValue = 0;
      m_AvgSum       = (TavgVal)0;
   }

/*********************************************************************************************
 Class      : Average
 Function   : void addValue(TavgVal val)
 Description: Adds a single value to the array

 Parameters:
  val: (E): Value to add  (TavgVal)

 Result type:  (void)
 created: June, 16 2005
 <TITLE addValue>
*********************************************************************************************/
   void addValue(TavgVal val)
   {
      m_AvgSum  = (TavgVal)(m_AvgSum + val);      // add the current value
      if (m_nNoOfAvgValues == ARBITRARY_AVG_VAL)
      {                                 // if the size is arbitrary
         m_AvgValues.push_back(val);
         m_nCurrenSize++;                  // allocate the desired memory just in time
      }
      else
      {
         if (m_nCurrenSize < m_nNoOfAvgValues)   // not full
         {
            m_AvgValues.push_back(val);   // insert the value
            m_nCurrenSize++;               // increase current size
            m_nCurrentValue++;               // increase current value
         }
         else                           // full
         {                              // subtract the last value
            if (m_nCurrentValue >= m_nCurrenSize)// last value
            {
               m_nCurrentValue = 0;         // go to first
            }
            m_AvgSum = (TavgVal)(m_AvgSum - m_AvgValues[m_nCurrentValue]);
            m_AvgValues[m_nCurrentValue] = val;// overwrite the value
            m_nCurrentValue++;
         }
      }
   }
   //////////////////////////////////////////////////////////////////////////
/*********************************************************************************************
 Class      : Average
 Function   : void setNoOfAvgValues(int nValues)
 Description: Sets the number of values in the array. If the last value in the array is set,
              the first value is overwritten.
              If ARBITRARY_AVG_VAL is set, the number of values is arbitrary.

 Parameters:
  nValues: (): Number of values in the array.  (int)

 Result type:  (void)
 created: June, 16 2005
 <TITLE setNoOfAvgValues>
*********************************************************************************************/
   void setNoOfAvgValues(int nValues)
   {
      clear();
      if (nValues < 1)   // number of values is arbitrary
      {
         m_nNoOfAvgValues = ARBITRARY_AVG_VAL;
      }
      else            // certain number of values
      {               // are allocated at the beginning to avoid memory fragmentation
         m_nNoOfAvgValues = nValues;
         m_AvgValues.reserve(m_nNoOfAvgValues);
      }
   }
   //////////////////////////////////////////////////////////////////////////
   // Access
/*********************************************************************************************
 Class      : Average
 Function   : TavgVal GetAtFast(int nPos)
 Description: retrieves a value in the array

 Parameters:
  nPos: (E): index of the value  (int)

 Result type:  (TavgVal)
 created: June, 16 2005
 <TITLE getAtFast>
*********************************************************************************************/
   TavgVal getAtFast(int nPos) const
   {
      assert(nPos >= 0 && nPos < m_nCurrenSize);
      return m_AvgValues[nPos];
   }
/*********************************************************************************************
 Class      : Average
 Function   : TavgVal getSum()
 Description: retrieves the sum of the values in the array.

 Parameters: None

 Result type:  (double), (TavgVal)
 created: June, 16 2005
 <TITLE  getSum()>
*********************************************************************************************/
   TavgVal getSum() const
   {
      return m_AvgSum;
   }
/*********************************************************************************************
 Class      : Average
 Function   : double GetAverage(), TavgVal GetAverageT()
 Description: retrieves the average of the values in the array.

 Parameters: None

 Result type:  (double), (TavgVal)
 created: June, 16 2005
 <TITLE  getAverage()>
*********************************************************************************************/
   double getAverage() const
   {
      if (m_nCurrenSize > 1)   // calculate the average value
      {
         double dSum = double(m_AvgSum), dDiv = double(m_nCurrenSize);
         return dSum / dDiv;
      }
      return (double)m_AvgSum;
   }

   TavgVal getAverageT() const
   {
      if (m_nCurrenSize > 1)   // calculate the average value
      {
         TavgVal dDiv = TavgVal(m_nCurrenSize);
         return m_AvgSum / dDiv;
      }
      return m_AvgSum;
   }
//////////////////////////////////////////////////////////////////////////
/*********************************************************************************************
 Class      : Average
 Function   : double getStandardDeviation()
 Description: calculates the standard deviation of the values in the array.

 Parameters: None

 Result type:  (double)
 created: June, 16 2005
 <TITLE double GetStandardDeviation>
*********************************************************************************************/
   double getStandardDeviation()
   {
      return sqrt(getVariance());
   }
/*********************************************************************************************
 Class      : Average
 Function   : double getVariance()
 Description: calculates the variance of the values in the array

 Parameters: None

 Result type:  (double)
 created: June, 16 2005
 <TITLE double GetVariance>
*********************************************************************************************/
   double getVariance()
   {
      if (m_nCurrenSize > 1)
      {
         return (getQDiff() / (double)(m_nCurrenSize));
      }
      return (double)0;
   }
/*********************************************************************************************
 Class      : Average
 Function   : double getQDiff(), TavgVal getQDiffT()
 Description: calculates the square difference sum of the values in the array

 Parameters: None

 Result type:  (double), (TavgVal)
 created: June, 16 2005
 <TITLE double GetVariance>
*********************************************************************************************/
   double getQDiff()
   {
      int i;
      double dSum = 0, dDiff, dAvg = getAverage();
      for (i=0; i<m_nCurrenSize; i++)
      {
         dDiff = (double)getAtFast(i) - dAvg;
         dSum += dDiff*dDiff;
      }
      return dSum;
   }
   TavgVal getQDiffT()
   {
      int i;
      TavgVal dSum = 0, dDiff, dAvg = getAverageT();
      for (i=0; i<m_nCurrenSize; i++)
      {
         dDiff = getAtFast(i) - dAvg;
         dSum += dDiff*dDiff;
      }
      return dSum;
   }
   //////////////////////////////////////////////////////////////////////////
/*********************************************************************************************
 Class      : Average
 Function   : int GetActualValues()
 Description: retrieves the actual number of values in the array

 Parameters: None

 Result type:  (int)
 created: June, 16 2005
 <TITLE GetActualValues>
*********************************************************************************************/
   int     getActualValues()  { return m_nCurrenSize;}
//////////////////////////////////////////////////////////////////////////
/*********************************************************************************************
 Class      : Average
 Function   : int GetNoOfAvgValues()
 Description: retrieves the array size

 Parameters: None

 Result type:  (unsigned int  )
 created: June, 16 2005
 <TITLE getNoOfAvgValues>
*********************************************************************************************/
   unsigned int   getNoOfAvgValues() const { return m_nNoOfAvgValues;}

/*********************************************************************************************
 Class      : Average
 Function   : int retNoOfAvgValues()
 Description: retrieves the maximum value in this array size

 Parameters: None

 Result type:  (unsigned int  )
 created: June, 16 2005
 <TITLE getNoOfAvgValues>
*********************************************************************************************/
   TavgVal getMax() const
   {
      int i;
      TavgVal Max = 0;
      for (i=0; i<m_nCurrenSize; i++)
      {
         Max = max(getAtFast(i), Max);
      }
      return Max;
   }

protected:
   //////////////////////////////////////////////////////////////////////////

private:
   //////////////////////////////////////////////////////////////////////////
   int      m_nNoOfAvgValues;
   int       m_nCurrentValue;
   int       m_nCurrenSize;
   TavgVal  m_AvgSum;
   std::vector<TavgVal> m_AvgValues;
};

template <class TavgVal>
void calculateFloatingAverage(const std::vector<TavgVal>& aInput, int aValues, std::vector<TavgVal>& aOutput)
{
    Average<TavgVal> fAvg;
    aValues      |= 1; // Anzahl der Werte soll ungerade sein
    fAvg.setNoOfAvgValues(aValues);
    int fBoundary = aValues / 2;
    int fSize     = aInput.size();
    int fUpper    = fSize - fBoundary;
    aOutput.resize(fSize);
    for (int i=0; i<fSize; ++i)
    {
        fAvg.addValue(aInput[i]);
        if (i >= fBoundary && i <fUpper)
        {
            aOutput[i-fBoundary] = fAvg.getAverageT();
        }
    }
}

#endif
