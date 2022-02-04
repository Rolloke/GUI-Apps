

import java.awt.event.*;
import javax.swing.*;

import CalculatorDialog.CalculatorDialog;


public class MainClass {
	public static void main(String[] args) 
	{
		JFrame frame = new JFrame("Calculator");
		WindowListener wl = new WindowAdapter() 
		{
			public void windowClosing( WindowEvent e )
			{
				System.exit(0); 
			}
		};
		frame.addWindowListener( wl );
		frame.getContentPane().add( new CalculatorDialog() );
		frame.pack();
		frame.setVisible(true);	
	}

}

