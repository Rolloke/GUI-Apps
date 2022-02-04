package myMath;

import java.math.BigInteger;

public class FractionBigInteger {

	public FractionBigInteger()
	{
		mNumerator = BigInteger.ZERO;
		mDenominator = BigInteger.ONE;
	}
	public FractionBigInteger(BigInteger aNumerator, BigInteger aDenominator)
	{
		mNumerator = aNumerator;
		mDenominator = aDenominator;
		reduce();
	}
	public FractionBigInteger(String aNumerator, String aDenominator)
	{
		mNumerator   = new BigInteger(aNumerator);
		mDenominator = new BigInteger(aDenominator);
		reduce();
	}

	BigInteger getNumerator()
	{
		return mNumerator;
	}

	BigInteger getDenominator()
	{
		return mDenominator;
	}

	public String toString()
	{
		String s = mNumerator + " / " + mDenominator;
		return s;
	}
	
	public double getValue()
	{
		return mNumerator.doubleValue() / mDenominator.doubleValue();
	}
	
	// sum
	public void add(BigInteger aValue)
	{
		aValue = aValue.multiply(mDenominator);
		mNumerator = mNumerator.add(aValue);
		reduce();
	}
	public void add(FractionBigInteger aValue)
	{
		mNumerator = mNumerator.multiply(aValue.mDenominator);
		BigInteger fTemp = aValue.mNumerator.multiply(mDenominator);
		mNumerator = mNumerator.add(fTemp);
		mDenominator = mDenominator.multiply(aValue.mDenominator);
		reduce();
	}

	// difference
	public void sub(BigInteger aValue)
	{
		mNumerator =  aValue.multiply(mDenominator);
		reduce();
	}
	public void sub(FractionBigInteger aValue)
	{
		mNumerator = mNumerator.multiply(aValue.mDenominator);
		BigInteger fTemp = aValue.mNumerator.multiply(mDenominator);
		mNumerator.subtract(fTemp);
		mDenominator = mDenominator.multiply(aValue.mDenominator);
		reduce();
	}

	// product
	public void mul(BigInteger aValue)
	{
		mNumerator = mNumerator.multiply(aValue);
		reduce();
	}
	public void mul(FractionBigInteger aValue)
	{
		mNumerator   = mNumerator.multiply(aValue.mNumerator);
		mDenominator = mDenominator.multiply(aValue.mDenominator);
		reduce();
	}

	// division
	public void div(BigInteger aValue)
	{
		mDenominator = mDenominator.multiply(aValue);
		reduce();
	}
	
	public void div(FractionBigInteger aValue)
	{
		mNumerator   = mNumerator.multiply(aValue.mDenominator);
		mDenominator = mDenominator.multiply(aValue.mNumerator);
		reduce();
	}
	

	private void reduce()
	{
		if (mDenominator.signum() < 0)
		{
			mDenominator = mDenominator.negate();
			mNumerator   = mNumerator.negate();
		}
		BigInteger div = gcd(mNumerator, mDenominator);
		int n= div.compareTo(BigInteger.ONE);
		if (n > 0)
		{
			mNumerator   = mNumerator.divide(div);
			mDenominator = mDenominator.divide(div);
		}
	}
	
	private BigInteger gcd(BigInteger a, BigInteger b)
	{
		if (a.compareTo(BigInteger.ZERO) == 0)
		{
			return b;
		}
		if (a.signum() < 0) a = a.negate();
		while (b.compareTo(BigInteger.ZERO) != 0)
		{
			if (a.compareTo(b) > 0) a = a.subtract(b);
			else                    b = b.subtract(a);
		};
		return a;
	}

	BigInteger mNumerator;
	BigInteger mDenominator;

}
