package myMath;

public class Statistics 
{
	public Statistics(String aTxtValues, String aSeparator)
	{
		mAverage = 0;
		mSquareAverage = 0;
		mGeomAverage = 0;
		mSigma = 0;
		mSquareSigma = 0;
		mValues  = null;
		String []fTxtValues = aTxtValues.split(aSeparator);
		int i, n = fTxtValues.length;
		if (n > 1)
		{
			mValues = new double[n];
			mSum = 0;
			mSquareSum = 0;
			mProduct = 1;
			for (i=0; i<n; i++)
			{
				mValues[i] = Double.valueOf(fTxtValues[i]);
				mSum      += mValues[i];
				mSquareSum+= mValues[i]*mValues[i];
				mProduct  *= mValues[i];
			}
			mAverage       = mSum / (double)n;
			mSquareAverage = Math.sqrt(mSquareSum / (double)n);
			mGeomAverage   = Math.pow(mProduct, 1.0/(double)n);
			double fTemp, fSum=0;
			for (i=0; i<n; i++)
			{
				fTemp = mValues[i] - mAverage;
				fSum += fTemp*fTemp;
			}
			mSquareSigma = fSum/(double)(n-1);
			mSigma = Math.sqrt(mSquareSigma);
		}	
	}
	
	public int getSize() 
	{
		if (mValues != null) return mValues.length;
		return 0;
	}
	public double getSum() { return mSum; }
	public double getSquareSum() { return mSquareSum; }
	public double getAverage() { return mAverage; }
	public double getSquareAverage() { return mSquareAverage; }
	public double getGeomAverage() { return mGeomAverage; }
	public double getSigma() { return mSigma; }
	public double getSqareSigma() { return mSquareSigma; }

	private double []mValues;
	private double mSum;
	private double mSquareSum;
	private double mProduct;
	private double mAverage;
	private double mSquareAverage;
	private double mGeomAverage;
	private double mSigma;
	private double mSquareSigma;

}
