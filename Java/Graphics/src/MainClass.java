import javax.swing.*;

import graphic.ViewPanel;


public class MainClass {
	public static void main(String[] args) 
	{
		JFrame frame = new JFrame("View Panel");
		frame.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
		frame.setSize(200, 150);
		frame.add(new ViewPanel(true) );
		frame.setVisible(true);	
	}

}

