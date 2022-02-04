package CalculatorDialog;

import java.awt.Component;
import java.awt.GridBagConstraints;
import java.awt.GridBagLayout;
import java.awt.event.ActionEvent;
import java.math.BigInteger;

import javax.swing.*;

import myMath.*;
import myMath.Arithmetic.*;


public class CalculatorDialog extends JPanel 
{
	private static final long serialVersionUID = 1L;
	private class TiA extends AbstractAction
	{
		private static final long serialVersionUID = 1L;

		TiA(String aAction)
		{
			super(aAction);
		}

		@Override public void actionPerformed(ActionEvent aEvent)
		{
			insertIntoTextfield(aEvent.getActionCommand());
		}
	}

	private class CmdA extends AbstractAction
	{
		private static final long serialVersionUID = 1L;

		CmdA(String aAction)
		{
			super(aAction);
		}

		@Override public void actionPerformed(ActionEvent aEvent)
		{
			String sCmd = aEvent.getActionCommand();
			if (sCmd.compareTo(mClearCmd) == 0)
			{
				mTextField.setText("");
				mStatValues = "";
			}
			else if (sCmd.compareTo(mCalcCmd) == 0)
			{   
				calculateExpression();
			}
			else if (sCmd.compareTo(mFacultyCmd) == 0)
			{
				calculateFaculty();
			}
			else if (sCmd.compareTo(mSelectCmd) == 0)
			{
				selectCmdButtons();
			}
			else if (  mFunctionSelector.getSelectedIndex() == 3
					|| mFunctionSelector.getSelectedIndex() == 4)
			{
				insertIntoTextfield(sCmd);
			}
			else
			{
				calculateCmds(aEvent.getActionCommand());
			}
		}
		
		private void calculateCmds(String aCmd)
		{
			int n = getIndex(mStatisticCmds, aCmd);
			if (n != -1)
			{
				switch (eStatistic.values()[n])
				{
				case separate: insertIntoTextfield(";"); break;
				case show: 
					mTextField.setText(mStatValues);
					mStatValues = "";
				break;
				default: calculateStatistic(eStatistic.values()[n]); break;
				}

			}
			else if (calculateExpression())
			{
				double fValue = 0;
				try
				{
					fValue = Double.valueOf(mTextField.getText());
				}
				catch(Exception e)
				{
					mTextField.setText("Error: " + e.getLocalizedMessage());
					return;
				}
				double fResult = 0;
				try
				{
    				n = getIndex(mStandardCmds, aCmd);
					if (n!=-1)
					{
						switch (eStandard.values()[n])
						{
						case sin:    fResult = Math.sin(fValue);     break;
						case cos:    fResult = Math.cos(fValue);     break;
						case tan:    fResult = Math.tan(fValue);     break;
						case ln:     fResult = Math.log(fValue);     break;
						case log:    fResult = Math.log10(fValue);   break;
						case sinh:   fResult = Math.sinh(fValue);    break;
						case cosh:   fResult = Math.cosh(fValue);    break;
						case tanh:   fResult = Math.tanh(fValue);    break;
						case Xsquare:fResult = fValue*fValue;        break;
						case Xqube:  fResult = fValue*fValue*fValue; break;
						}
					}
					else
					{
						n = getIndex(mInverseCmds, aCmd);
						switch (eInverse.values()[n])
						{
						case asin:  fResult = Math.asin(fValue);    break;
						case acos:  fResult = Math.acos(fValue);    break;
						case atan:  fResult = Math.atan(fValue);    break;
						case e_x:   fResult = Math.exp(fValue);     break;
						case _10_x: fResult = Math.pow(10, fValue); break;
						case asinh: fResult = Math.log(fValue + Math.sqrt(fValue * fValue + 1)); break;
						case acosh: fResult = Math.log(fValue + Math.sqrt(fValue * fValue - 1)); break;
						case atanh: fResult = 0.5 * Math.log((1.0 + fValue) / (1.0 - fValue)); break;
						case sqrt:  fResult = Math.sqrt(fValue);    break;
						case Xqube: fResult = Math.pow(fValue, 1.0/3.0); break;
						}
					}
				}
				catch (Exception e)
				{
					mTextField.setText("Error: " + e.getLocalizedMessage());
					return;
				}
				mTextField.setText(Double.toString(fResult));
			}
		}
		
		private int getIndex(String  aList[], String aId)
		{
			for (int i=0; i<aList.length; i++)
			{
				if (aId.compareTo(aList[i])==0)
				{
					return i;
				}
			}
			return -1;
		}
	}
	
	public CalculatorDialog()
	{
		InitDlgWithGridBagLayout();
	}
	
	private boolean calculateExpression()
	{
		try
		{
			mArithmetic.setExpression(mTextField.getText());
		}
		catch(ExpressionException e)
		{
			mTextField.setSelectionStart(e.mStart);
			mTextField.setSelectionEnd(e.mEnd);
			JOptionPane.showMessageDialog(this, "Error: " + e.getMessage()); 
			return false;
		}
		double fResult = mArithmetic.calculate();
		mTextField.setText(Double.toString(fResult));
		return true;
	}
	
	private void calculateFaculty()
	{
		BigInteger fResult;
		try
		{
			fResult = BigInteger.ONE;
			long i;
			long n = Integer.valueOf(mTextField.getText());
			for (i=1;i<=n; i++)
			{
				fResult = fResult.multiply(BigInteger.valueOf(i));
			}
		}
		catch(Exception e)
		{
			mTextField.setText("Error: " + e.getLocalizedMessage());
			return;
		}
		mTextField.setText(fResult.toString());
	}
	
	private void calculateStatistic(eStatistic aStat)
	{
		if (mStatValues.length() == 0)
		{
			mStatValues = mTextField.getText();
			mStatistics = new Statistics(mStatValues, ";");
		}
		if (mStatistics != null && mStatistics.getSize() > 1)
		{
			switch(aStat)
			{
			case sum: // sum of elements
				mTextField.setText(Double.toString(mStatistics.getSum()));
				break;
			case sqsum: // square sum of elements 
				mTextField.setText(Double.toString(mStatistics.getSquareSum()));
				break;
			case avg: // arithmetic average
				mTextField.setText(Double.toString(mStatistics.getAverage()));
				break;
			case qavg:// square average
				mTextField.setText(Double.toString(mStatistics.getSquareAverage()));
				break;
			case gavg: // geometric average
				mTextField.setText(Double.toString(mStatistics.getGeomAverage()));
				break;
			case count: // number of elements
				mTextField.setText(Integer.toString(mStatistics.getSize())); 
				break;
			case sigma: // standard deviation
				mTextField.setText(Double.toString(mStatistics.getSigma())); 
				break;
			case qsigma: // square standard deviation
				mTextField.setText(Double.toString(mStatistics.getSqareSigma()));
				break;
			}
		}
		else
		{
			mTextField.setText("enter values separated by semicolon"); 
		}
	}
	
	private void insertIntoTextfield(String s)
	{
		int fSelection = 0;
		fSelection = mTextField.getText().length();
		mTextField.setSelectionStart(fSelection);
		mTextField.setSelectionEnd(fSelection);
		mTextField.replaceSelection(s);
	}
	
	private void selectCmdButtons()
	{
		String fSeleted = (String)mFunctionSelector.getSelectedItem();
		if (fSeleted == mInverseCmd)
		{
			for (int i=0; i<10; i++)
			{
				mCommandButtons[i].setText(mInverseCmds[i]);
				mCommandButtons[i].setToolTipText("");
			}
		}
		else if (fSeleted == mStandardCmd)
		{
			for (int i=0; i<10; i++)
			{
				mCommandButtons[i].setText(mStandardCmds[i]);
				mCommandButtons[i].setToolTipText("");
			}
		}
		else if (fSeleted == mStatisticCmd)
		{
			for (int i=0; i<10; i++)
			{
				mCommandButtons[i].setText(mStatisticCmds[i]);
				mCommandButtons[i].setToolTipText(mStatisticHelp[i]);
			}
		}
		else if (fSeleted == mConstant1)
		{
			for (int i=0; i<10; i++)
			{
				mCommandButtons[i].setText(Arithmetic.Constants.getName(i));
				mCommandButtons[i].setToolTipText(Arithmetic.Constants.getHelp(i));
			}
		}
		else if (fSeleted == mConstant2)
		{
			for (int i=0; i<10; i++)
			{
				mCommandButtons[i].setText(Arithmetic.Constants.getName(i+10));
				mCommandButtons[i].setToolTipText(Arithmetic.Constants.getHelp(i+10));
			}
		}
			
	}
	
	private void InitDlgWithGridBagLayout()
	{
		GridBagLayout gbl = new GridBagLayout(); 
		setLayout(gbl);
		
		mTextField  = new JTextField("");
		mArithmetic = new Arithmetic();
		mStatValues = "";

		int line = 0, n = 0, size = mStandardCmds.length;

		mCommandButtons = new JButton[size];

		gbl.columnWidths = new int[6];
		for (n=0; n<6; n++)
		{
			gbl.columnWidths[n] = 80; 
		}
		
		addComponent(gbl, mTextField, 0, line++, 6, 1, 1.0, 1.0);
		mTextField.setAlignmentX(RIGHT_ALIGNMENT);
		
		mFunctionSelector = new JComboBox();
		mFunctionSelector.addItem(mStandardCmd);
		mFunctionSelector.addItem(mInverseCmd);
		mFunctionSelector.addItem(mStatisticCmd);
		mFunctionSelector.addItem(mConstant1);
		mFunctionSelector.addItem(mConstant2);
		mFunctionSelector.addActionListener(new CmdA(mSelectCmd));
		mFunctionSelector.setActionCommand(mSelectCmd);
    	addComponent(gbl, mFunctionSelector, 0, line, 1, 1, 0.0, 1.0 );
    	
		for (n=0; n<(size/2); n++)
		{
			mCommandButtons[n] = new JButton(new CmdA(mStandardCmds[n]));
			addComponent(gbl, mCommandButtons[n], n+1, line, 1, 1, 0.0, 1.0 );
		}
		line++;
		addComponent(gbl, new JButton(new CmdA(mFacultyCmd)), 0, line, 1, 1, 0.0, 1.0 );
		for (; n<size; n++)
		{
			mCommandButtons[n] = new JButton(new CmdA(mStandardCmds[n]));
			addComponent(gbl, mCommandButtons[n], n-4, line, 1, 1, 0.0, 1.0 );
		}
		line++;
		addComponent(gbl, new JButton(new TiA("1")), 0, line, 1, 1, 1.0, 1.0 );
		addComponent(gbl, new JButton(new TiA("2")), 1, line, 1, 1, 1.0, 1.0 );
		addComponent(gbl, new JButton(new TiA("3")), 2, line, 1, 1, 1.0, 1.0 );
		addComponent(gbl, new JButton(new TiA("(")), 4, line, 1, 1, 1.0, 1.0 );
		addComponent(gbl, new JButton(new TiA(")")), 5, line, 1, 1, 1.0, 1.0 );
		addComponent(gbl, new JButton(new TiA("+")), 3, line, 1, 1, 1.0, 1.0 );
		line++;
		addComponent(gbl, new JButton(new TiA("4")), 0, line, 1, 1, 1.0, 1.0 );
		addComponent(gbl, new JButton(new TiA("5")), 1, line, 1, 1, 1.0, 1.0 );
		addComponent(gbl, new JButton(new TiA("6")), 2, line, 1, 1, 1.0, 1.0 );
		addComponent(gbl, new JButton(new TiA("^")), 4, line, 1, 1, 1.0, 1.0 );
		addComponent(gbl, new JButton(new TiA("e")), 5, line, 1, 1, 1.0, 1.0 );
		addComponent(gbl, new JButton(new TiA("-")), 3, line, 1, 1, 1.0, 1.0 );
		line++;
		addComponent(gbl, new JButton(new TiA("7")), 0, line, 1, 1, 1.0, 1.0 );
		addComponent(gbl, new JButton(new TiA("8")), 1, line, 1, 1, 1.0, 1.0 );
		addComponent(gbl, new JButton(new TiA("9")), 2, line, 1, 1, 1.0, 1.0 );
		addComponent(gbl, new JButton(new TiA("*")), 3, line, 1, 1, 1.0, 1.0 );
		addComponent(gbl, new JButton(new TiA(" ")), 4, line, 1, 1, 1.0, 1.0 );
		addComponent(gbl, new JButton(new CmdA(mClearCmd)), 5, line, 1, 1, 1.0, 1.0 );
		line++;
		addComponent(gbl, new JButton(new TiA("0")), 0, line, 2, 1, 1.0, 1.0 );
		addComponent(gbl, new JButton(new TiA(".")), 2, line, 1, 1, 1.0, 1.0 );
		addComponent(gbl, new JButton(new TiA("/")), 3, line, 1, 1, 1.0, 1.0 );
		addComponent(gbl, new JButton(new CmdA(mCalcCmd)), 4, line, 2, 1, 1.0, 1.0 );
	}
	
	private void addComponent(GridBagLayout gbl, Component c, 
			int x, int y, int width, int height, double weight_x, double weight_y)
	{
		GridBagConstraints gbc = new GridBagConstraints();
		gbc.fill       = GridBagConstraints.BOTH;
		gbc.gridx      = x; 		gbc.gridy      = y;
		gbc.gridwidth  = width;		gbc.gridheight = height;
		gbc.weightx    = weight_x;	gbc.weighty    = weight_y;
		gbl.setConstraints(c, gbc);
		add(c);
	}
		
	JTextField mTextField;
	Arithmetic mArithmetic;
	JButton []   mCommandButtons;
	JComboBox    mFunctionSelector;
	String       mStatValues;
	Statistics   mStatistics;
	final String mClearCmd     = "Clear";
	final String mCalcCmd      = "Calc";
	final String mSelectCmd    = "Select";
	final String mFacultyCmd   = "n!";
	final String mStandardCmd  = "Std";
	final String mInverseCmd   = "Inv";
	final String mStatisticCmd = "Stat";
	final String mConstant1     = "Cnst 1";
	final String mConstant2     = "Cnst 2";
	
	final String mStandardCmds[]  = {"sin", "cos", "tan", "ln", "log", "sinh", "cosh", "tanh", "x²"   , "x³" };
	private enum eStandard          { sin ,  cos ,  tan ,  ln ,  log ,  sinh ,  cosh ,  tanh , Xsquare, Xqube };
	final String mInverseCmds[]   = {"asin", "acos", "atan ", "e^x ", "10^x", "asinh", "acosh", "atanh",  "√²", "√³" };
	private enum eInverse           { asin ,  acos ,  atan ,   e_x  , _10_x ,  asinh ,  acosh ,  atanh ,  sqrt, Xqube };
	final String mStatisticCmds[] = { ";"     , "n"  , "values",  "∑", "∑²" , "Avg", "Avg²", "g Avg",   "σ" ,  "σ²" };
	private enum eStatistic         { separate, count,  show   ,  sum, sqsum,  avg ,  qavg ,   gavg ,  sigma, qsigma};
	final String mStatisticHelp[] = { "Separator", "number of values", "show values",
			"calculates sum", "calculates square sum" , "arithmetic average", "square average",
			"geometrical average",   "standard deviation" ,  "square standard deviation" };
}

/*
	private void InitDlgWithGridLayout()
	{
		setLayout(new GridLayout(5,4));
		add( new JButton( "1" ) );
		add( new JButton( "2" ) );
		add( new JButton( "3" ) );
		add( new JButton( "4" ) );
		add( new JButton( "5" ) );
		add( new JButton( "6" ) );
		add( new JButton( "7" ) );
		add( new JButton( "8" ) );
		add( new JButton( "9" ) );
		add( new JButton( "0" ) );
		add( new JButton( "+" ) );
		add( new JButton( "-" ) );
		add( new JButton( "*" ) );
		add( new JButton( "/" ) );
		add( new JButton( "CALC" ) );
	}
*/