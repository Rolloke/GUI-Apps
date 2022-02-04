
import javax.swing.JFileChooser;
import javax.swing.JOptionPane;

import myMath.*;

public class MainClass
{

	/**
	 * @param args
	 */
	public static void main(String[] args) 
	{
		testOptionPaneDialogs();
	}
	
	private static void testOptionPaneDialogs()
	{
		//UIManager.setInstalledLookAndFeels(com.sun.java.swing.plaf.gtk.resources.metacity.SwingFallbackTheme);
		String[] options = { "A Message", "Input", "Confirm", "Open File", "Save File", "Fraction"}; 
		int o = JOptionPane.showOptionDialog(null, "wähle!", "Hello World", 0, 0, null, options, options[0]);
		String fAnswer="";
		int n = 0;
		switch(o)
		{
		case 0: JOptionPane.showMessageDialog(null, "Send me a message!"); break;
		case 1: fAnswer = JOptionPane.showInputDialog(null, "Gib was ein!"); break;
		case 2: n = JOptionPane.showConfirmDialog(null, "Bestätige dies!"); break;
		case 3:
		{
			JFileChooser fc = new JFileChooser();
			fc.showOpenDialog(null);
			fAnswer = fc.getSelectedFile().toString();
		}break;
		case 4:
		{
			JFileChooser fc = new JFileChooser();
			fc.showSaveDialog(null);
			fAnswer = fc.getSelectedFile().toString();
		}break;
		case 5: testFraction(); break;
		}
		
	    System.out.println(fAnswer);
		System.out.println(n);
		System.out.println(o);
	}
	
	private static void testFraction()
	{
		FractionBigInteger f1 = new FractionBigInteger("5","8");
		FractionBigInteger f2 = new FractionBigInteger("3","5");

		System.out.println(f1 + " * " + f2 + "=");
		f1.mul(f2);
		double fValue = f1.getValue();
		System.out.println(f1 + ": " + fValue);

		f1 = new FractionBigInteger("5","8");;
		System.out.println(f1 + " / " + f2 + "=");
		f1.div(f2);
		fValue = f1.getValue();
		System.out.println(f1 + ": " + fValue);

		f1 = new FractionBigInteger("5","8");
		System.out.println(f1 + " + " + f2 + "=");
		f1.add(f2);
		fValue = f1.getValue();
		System.out.println(f1 + ": " + fValue);

		f1 = new FractionBigInteger("5","8");
		System.out.println(f1 + " - " + f2 + "=");
		f1.sub(f2);
		fValue = f1.getValue();
		System.out.println(f1 + ": " + fValue);
		
		System.out.println(average(new Fraction(5,6), new Fraction(19,6), new Fraction(22,3)));
	}
	
	private static Fraction average(Fraction... values)
	{
		Fraction sum = new Fraction();
		for (Fraction d : values)
		{
			sum.add(d);
		}
		sum.div(values.length);
		return sum;
	}
	
	

}
