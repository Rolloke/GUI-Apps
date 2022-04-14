#ifndef _CAverage_H_INCLUDED_
#define _CAverage_H_INCLUDED_

#define ARBITRARY_AVG_VAL -1

#include <vector>

template <class TavgVal>

class CAverage
{
public:
   //////////////////////////////////////////////////////////////////////////
   // Konstruction / Destruction
/*********************************************************************************************
 Class      : CAverage
 Function   : CAverage()
 Description: Creates an array to store values of arbitrary types.
              Sample: CAverage<double> ArrayWithFloatingPointValues

 Parameters: None

 Result type:  ()
 created: June, 16 2005
 <TITLE CAverage>
*********************************************************************************************/
   CAverage()
   {
      m_nNoOfAvgValues = ARBITRARY_AVG_VAL;
      m_nCurrentValue  = 0;
      m_nCurrenSize     = 0;
      m_AvgSum           = (TavgVal)0;
   };
   //////////////////////////////////////////////////////////////////////////
   ~CAverage()
   {
      RemoveAll();
   };

   //////////////////////////////////////////////////////////////////////////
   // Implementation
/*********************************************************************************************
 Class      : CAverage
 Function   : void RemoveAll()
 Description: Frees all Data

 Parameters: None

 Result type:  (void)
 created: June, 16 2005
 <TITLE RemoveAll>
*********************************************************************************************/
   void RemoveAll()
   {
      m_AvgValues.clear();
      m_nCurrenSize   = 0;
      m_nCurrentValue = 0;
      m_AvgSum       = (TavgVal)0;
   };

/*********************************************************************************************
 Class      : CAverage
 Function   : void AddValue(TavgVal val)
 Description: Adds a single value to the array

 Parameters:
  val: (E): Value to add  (TavgVal)

 Result type:  (void)
 created: June, 16 2005
 <TITLE AddValue>
*********************************************************************************************/
   void AddValue(TavgVal val)
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
   };
   //////////////////////////////////////////////////////////////////////////
/*********************************************************************************************
 Class      : CAverage
 Function   : void SetNoOfAvgValues(int nValues)
 Description: Sets the number of values in the array. If the last value in the array is set,
              the first value is overwritten.
              If ARBITRARY_AVG_VAL is set, the number of values is arbitrary.

 Parameters:
  nValues: (): Number of values in the array.  (int)

 Result type:  (void)
 created: June, 16 2005
 <TITLE SetNoOfAvgValues>
*********************************************************************************************/
   void SetNoOfAvgValues(int nValues)
   {
      RemoveAll();
      if (nValues < 1)   // number of values is arbitrary
      {
         m_nNoOfAvgValues = ARBITRARY_AVG_VAL;
      }
      else            // certain number of values
      {               // are allocated at the beginning to avoid memory fragmentation
         m_nNoOfAvgValues = nValues;
         m_AvgValues.reserve(m_nNoOfAvgValues);
      }
   };
   //////////////////////////////////////////////////////////////////////////
   // Access
/*********************************************************************************************
 Class      : CAverage
 Function   : TavgVal GetAtFast(int nPos)
 Description: retrieves a value in the array

 Parameters:
  nPos: (E): index of the value  (int)

 Result type:  (TavgVal)
 created: June, 16 2005
 <TITLE GetAtFast>
*********************************************************************************************/
   TavgVal GetAtFast(int nPos) const
   {
      ASSERT(nPos >= 0 && nPos < m_nCurrenSize);
      return m_AvgValues[nPos];
   };
/*********************************************************************************************
 Class      : CAverage
 Function   : TavgVal GetSum()
 Description: retrieves the sum of the values in the array.

 Parameters: None

 Result type:  (double), (TavgVal)
 created: June, 16 2005
 <TITLE  GetSum()>
*********************************************************************************************/
   TavgVal GetSum() const
   {
      return m_AvgSum;
   };
/*********************************************************************************************
 Class      : CAverage
 Function   : double GetAverage(), TavgVal GetAverageT()
 Description: retrieves the average of the values in the array.

 Parameters: None

 Result type:  (double), (TavgVal)
 created: June, 16 2005
 <TITLE  GetAverage()>
*********************************************************************************************/
   double GetAverage()
   {
      if (m_nCurrenSize > 1)   // calculate the average value
      {
         double dSum = double(m_AvgSum), dDiv = double(m_nCurrenSize);
         return dSum / dDiv;
      }
      return (double)m_AvgSum;
   };
   TavgVal GetAverageT()
   {
      if (m_nCurrenSize > 1)   // calculate the average value
      {
         TavgVal dDiv = TavgVal(m_nCurrenSize);
         return m_AvgSum / dDiv;
      }
      return m_AvgSum;
   };
//////////////////////////////////////////////////////////////////////////
/*********************************************************************************************
 Class      : CAverage
 Function   : double GetStandardDeviation()
 Description: calculates the standard deviation of the values in the array.

 Parameters: None

 Result type:  (double)
 created: June, 16 2005
 <TITLE double GetStandardDeviation>
*********************************************************************************************/
   double GetStandardDeviation()
   {
      if (m_nCurrenSize > 2)
      {
         return sqrt(GetQDiff() / (double)(m_nCurrenSize-1));
      }
      else if (m_nCurrenSize > 1)
      {
         return sqrt(GetQDiff());
      }
      return (double)m_AvgSum;
   };
/*********************************************************************************************
 Class      : CAverage
 Function   : double GetVariance()
 Description: calculates the variance of the values in the array

 Parameters: None

 Result type:  (double)
 created: June, 16 2005
 <TITLE double GetVariance>
*********************************************************************************************/
   double GetVariance()
   {
      if (m_nCurrenSize > 1)
      {
         return sqrt(GetQDiff() / (double)(m_nCurrenSize));
      }
      return (double)m_AvgSum;
   };
/*********************************************************************************************
 Class      : CAverage
 Function   : double GetQDiff(), TavgVal GetQDiffT()
 Description: calculates the square difference sum of the values in the array

 Parameters: None

 Result type:  (double), (TavgVal)
 created: June, 16 2005
 <TITLE double GetVariance>
*********************************************************************************************/
   double GetQDiff()
   {
      int i;
      double dSum = 0, dDiff, dAvg = GetAverage();
      for (i=0; i<m_nCurrenSize; i++)
      {
         dDiff = (double)GetAtFast(i) - dAvg;
         dSum += dDiff*dDiff;
      }
      return dSum;
   };
   TavgVal GetQDiffT()
   {
      int i;
      TavgVal dSum = 0, dDiff, dAvg = GetAverageT();
      for (i=0; i<m_nCurrenSize; i++)
      {
         dDiff = GetAtFast(i) - dAvg;
         dSum += dDiff*dDiff;
      }
      return dSum;
   };
   //////////////////////////////////////////////////////////////////////////
/*********************************************************************************************
 Class      : CAverage
 Function   : int GetActualValues()
 Description: retrieves the actual number of values in the array

 Parameters: None

 Result type:  (int)
 created: June, 16 2005
 <TITLE GetActualValues>
*********************************************************************************************/
   int     GetActualValues()  { return m_nCurrenSize;};
//////////////////////////////////////////////////////////////////////////
/*********************************************************************************************
 Class      : CAverage
 Function   : int GetNoOfAvgValues()
 Description: retrieves the array size

 Parameters: None

 Result type:  (DWORD  )
 created: June, 16 2005
 <TITLE GetNoOfAvgValues>
*********************************************************************************************/
   DWORD   GetNoOfAvgValues() { return m_nNoOfAvgValues;};

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
#endif