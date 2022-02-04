/**
 * 
 */
package myMath;

/**
 * @author rolf
 *
 */
public class Fraction {

	public Fraction()
	{
		mNumerator = 0;
		mDenominator = 1;
	}
	public Fraction(Integer aNumerator, Integer aDenominator)
	{
		mNumerator = aNumerator;
		mDenominator = aDenominator;
		reduce();
	}

	Integer getNumerator()
	{
		return mNumerator;
	}

	Integer getDenominator()
	{
		return mDenominator;
	}

	public String toString()
	{
		String s = mNumerator + " / " + mDenominator;
		return s;
	}
	
	double getValue()
	{
		return (double)mNumerator / (double)mDenominator;
	}
	// sum
	public void add(Integer aValue)
	{
		mNumerator += aValue*mDenominator;
		reduce();
	}
	public void add(Fraction aValue)
	{
		mNumerator = mNumerator*aValue.mDenominator + aValue.mNumerator*mDenominator;
		mDenominator *=  aValue.mDenominator;
		reduce();
	}

	// difference
	public void sub(Integer aValue)
	{
		mNumerator -= aValue*mDenominator;
		reduce();
	}
	public void sub(Fraction aValue)
	{
		mNumerator = mNumerator*aValue.mDenominator - aValue.mNumerator*mDenominator;
		mDenominator *=  aValue.mDenominator;
		reduce();
	}

	// product
	public void mul(Integer aValue)
	{
		mNumerator *= aValue;
		reduce();
	}
	
	public void mul(Fraction aValue)
	{
		mNumerator *= aValue.mNumerator;
		mDenominator *=  aValue.mDenominator;
		reduce();
	}

	// division
	public void div(Integer aValue)
	{
		mDenominator *= aValue;
		reduce();
	}
	public void div(Fraction aValue)
	{
		mNumerator *= aValue.mDenominator;
		mDenominator *=  aValue.mNumerator;
		reduce();
	}

	private void reduce()
	{
		if (mDenominator < 0)
		{
			mDenominator = -mDenominator;
			mNumerator   = -mNumerator;
		}
		Integer div = gcd(mNumerator, mDenominator);
		if (div > 1)
		{
			mNumerator /= div;
			mDenominator /= div;
		}
	}
	
	private Integer gcd(Integer a, Integer b)
	{
		if (a == 0)
		{
			return b;
		}
		if (a < 0) a = -a;
		while (b != 0)
		{
			if (a > b) a = a - b;
			else       b = b - a;
		};
		return a;
	}

	Integer mNumerator;
	Integer mDenominator;

}
