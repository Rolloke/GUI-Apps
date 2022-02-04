/**
 * 
 */
package myMath;

/**
 * @author rolf
 *
 */
public class Arithmetic {

	public class ExpressionException extends RuntimeException
	{
		private static final long serialVersionUID = 1L;
		public ExpressionException()
		{
			super();
		}
		public ExpressionException(String sMsg, int aStart, int aEnd)
		{
			super(sMsg);
			mStart = aStart;
			mEnd   = aEnd;
		}
		public int mStart;
		public int mEnd;
	}

	public static class Constants 
	{
		public static String getName(int i)
		{
			return mNames[i];
		}
		public static String getHelp(int i)
		{
			return mHelp[i];
		}
		
		public static double getValue(String s)
		{
			int i, n = mNames.length;
			for (i=0; i<n; i++)
			{
				if (s.compareTo(mNames[i]) == 0)
				{
					return mValues[i];
				}
			}
			return 0;
		}
		private final static String mNames[] = {
			"Π",  "µ0", "ε0", "εc", "h" , "k" , "G" , "c"  , "α"  , "F"  ,  
			"NA", "me", "mp", "mn", "ma", "re", "a0", "mol", "mme", "mmp", "e"  };
		private final static String mHelp[] = {
			"PI constant",
			"permeability of vacuum",
			"dielectricity of vacuum",
			"unit charge",
			"plank constant",
			"boltzman constant",
			"gravity",
			"speed of light",
			"fine structure constant",
			"faraday constant",
			"avogadro constant",
			"electron rest mass",
			"proton mass",
			"neutron mass",
			"atom mass",
			"electron radius",
			"bohr radius", 
			"mol volume",
			"magnetic moment of electron",
			"magnetic moment of proton"};
		private final static double mValues[] = {
			Math.PI,
			1.2566370613999999e-6,
			8.8541878170000005e-12,
			1.6021773299999999e-19,
			6.6260755e-34,
			1.3806580000000001e-23,
			9.8066499999999994,
			2.99792458e8,
			7.2973530796448189e-3,
			9.6484600000000006e4,
			6.0221412927e23,
			9.1093829140e-31,
			1.67262158e-27,
			1.6749e-27,
			0.00091e-27,
			1.660538782e-27,
			2.817940289e-15,
			0.52917721092e-10,
			6.022e23,
			-928.476377e-26,
			1.410606743e-26,
			Math.E
			};
	}
	
	private class Node
	{
		public Node() 
	    {
	    	mLevel = 0; mBrace = 0; mOperator = 0;
	    	mName  = null; mValue = 0.0;
	    	mLeft  = null; mRight = null;

	    }
		public Node(Node aSrc) 
	    {
	    	mLevel = aSrc.mLevel; mBrace = aSrc.mBrace; mOperator = aSrc.mOperator;
	    	mName  = aSrc.mName; mValue  = aSrc.mValue;
	    	mLeft  = null; mRight = null;
	    }
		
		public int getOrder()
		{
			return mLevel + mBrace * 256;// + mOperator << 16;
		}
		
	    int    mLevel;
	    int    mBrace;
	    int    mOperator;
	    String mName;
	    double mValue;
	    Node   mLeft;
	    Node   mRight;
	};

	public Arithmetic() {
		mRoot = null;
	}
	
	public int setExpression(String aExpression)
	{
		mRoot = null;
		if (aExpression == null || aExpression.length() == 0)
		{
			throw new ExpressionException("No expression", 0, 1);
		}
		Node fBinaryOperator = new Node();
		Node aNumber   = new Node();
		aNumber.mLevel = IDL_AR_NUMBER;
	    int fLastChar  = 0;
	    int fChar      = 0;
	    int fLevel     = 0;
	    String str     = "";
	    int i;
	    char [] fCharbuf = new char[1];

	    for (i=0; i<aExpression.length()+1; i++)
		{
			fLastChar = fChar;
			if (i<aExpression.length())
			{
				aExpression.getChars(i, i+1, fCharbuf, 0);
				fChar = fCharbuf[0]; 
			}
			else fChar = 0;
			fLevel    = 0;
			switch(fChar)
			{
			case '$': case '"': case '?': case ';': case ':': case '#':
				throw new ExpressionException("Invalid Character", i, i+1);
	         case '\n': case '\r': case ' ':
	        	 break;
	         case ')' : fBinaryOperator.mBrace--;
	            break;
	         case '(' : fBinaryOperator.mBrace++;
	         case '^' :                                   fLevel++; // Level = 5
	         case '/' : case '%' :                        fLevel++; // Level = 4
	         case '*' :            case '&' :             fLevel++; // Level = 3
	         case '-' :
	             if (   fChar == '-' 
	                 && (fLastChar=='e' || fLastChar == 'E' || isOperator(fLastChar) || fLastChar==0))
	             {
	                 str += (char)fChar;
	                 break;
	             }
	             else                                      fLevel++; // Level = 2   // Achtung nachträglich geändert !
	         case '+' : case '|' : case '!' :                      // Level = 1   // Testen
	         case '>' : case '<' : case '=' :
	             if (   fChar == '+' 
	                 && (fLastChar=='e' || fLastChar == 'E'))
	             {
	                 str += (char)fChar;
	                 break;
	             }
	             else                                      fLevel++; // Level = 2   // Achtung nachträglich geändert !
	         case  0  :
	        	 if (str.length()>0)
	        	 {
	        		 try
	        		 {
		        		 aNumber.mValue = Double.valueOf(str);
	        		 }
	        		 catch(Exception e)
	        		 {
	        			aNumber.mValue = Constants.getValue(str);
						if (aNumber.mValue == 0)
						{
	        			    throw new ExpressionException(e.toString(), i-1-str.length(), i-1);
						}
	        		 }
		        	 str = "";
	        		 if (aNumber.mBrace < fBinaryOperator.mBrace)
	        		 {
	        			 aNumber.mBrace = fBinaryOperator.mBrace;
	        		 }
		        	 mRoot = insert(mRoot, aNumber);

		        	 aNumber.mBrace = fBinaryOperator.mBrace;
		        	 
		        	 if (fChar == 0) break;
		        	 fBinaryOperator.mLevel = fLevel;
		        	 fBinaryOperator.mOperator = fChar;
		        	 mRoot = insert(mRoot, fBinaryOperator);
	        	 }
	        	 
	        	 break;
	         default:
	        	 str += (char)fChar;

			}
		}
	    if (fBinaryOperator.mBrace>0)
	    {
			throw new ExpressionException("Brace Error", 0, 1);
	    }
		return 0;
	}
	
	public double calculate()
	{
		return calculate(mRoot);
	}
	
	private boolean isOperator(int aChar)
	{
		switch(aChar)
		{
        case '^': case '/': case '%': case '*': case '&': case '-':
        case '+': case '|': case '!': case '>': case '<': case '=':
        	return true;
		}
		return false;
	}
	
	private double calculate(Node p)
	{
		if (p == null) return 0;
		if (p.mOperator <= IDO_AR_CONST) return p.mValue;
		double fLeft  = calculate(p.mLeft);
		double fRight = calculate(p.mRight);
		switch (p.mOperator)
		{
		case '+': return fLeft+fRight;
		case '-': return fLeft-fRight;
		case '*': return fLeft*fRight;
		case '/': return fLeft/fRight;
		case '^': return Math.pow(fLeft, fRight);
		case '%': return fLeft%fRight;
		case '<': return (fLeft<fRight) ? 1.0 : 0.0;
		case '>': return (fLeft>fRight) ? 1.0 : 0.0;
		case '=': return (fLeft==fRight) ? 1.0 : 0.0;
		case '&': return (fLeft != 0) && (fRight != 0) ? 1.0 : 0.0;
		case '|': return (fLeft != 0) || (fRight != 0) ? 1.0 : 0.0;
		}
		return 0;
	}
	
	private Node insert(Node p, final Node pin)
	{
	   Node q = null;

	   if (p == null)                           // Fall (1)
	   {
	      p = new Node(pin);                    // neues Element eingefügen
	   }
	   else if (pin.mOperator <= IDO_AR_CONST)  // ist es ein numerischer Wert
	   {
	      if (p.mLeft == null)                  // Fall (2)
	      {
	         p.mLeft = insert(p.mLeft, pin);    // linkes Blatt zuerst belegen
	      }
	      else if (p.mRight == null)            // Fall (3)
	      {
	         p.mRight = insert(p.mRight, pin);  // dann rechtes Blatt
	      }
	      else                                  // Fall (4)
	      {
	         p.mRight = insert(p.mRight, pin);  // weitersuchen
	      }
	   }
	   else                                     // wenn es ein Operator ist
	   {
	      if (pin.getOrder() <= p.getOrder())   // Fall (5)
	      {                                     // wenn der Rang niedriger als der Rang im Knoten oder gleich ist  
	         q = new Node(pin);                 // Knoten dazwischensetzen
	         q.mLeft = p;
	         return q;
	      }
	      else                                  // Fall (6)
	      {
	         p.mRight =insert(p.mRight, pin);   // weitersuchen
	      }  
	   }
	   return p;
	}
	
	private Node   mRoot;
	
	final int IDO_AR_NUM      = 0;
	final int IDO_AR_VAR      = 1;
	final int IDO_AR_CONST    = 2;
	final int IDL_AR_FUNCTION = 5;
	final int IDL_AR_NUMBER   = 6;
}
