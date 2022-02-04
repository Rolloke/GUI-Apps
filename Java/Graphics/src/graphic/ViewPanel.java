/**
 * 
 */
package graphic;

import java.awt.Graphics;
import java.awt.Graphics2D;
import java.awt.LayoutManager;
import java.awt.RenderingHints;
import java.awt.Shape;
import java.awt.geom.Line2D;
import java.util.Collection;

import javax.swing.JPanel;

/**
 * @author rolf
 *
 */
public class ViewPanel extends JPanel {

	/**
	 * 
	 */
	private static final long serialVersionUID = 1L;

	/**
	 * 
	 */
	public ViewPanel() {
		initialize();
	}

	/**
	 * @param layout
	 */
	public ViewPanel(LayoutManager layout) {
		super(layout);
		initialize();
	}

	/**
	 * @param isDoubleBuffered
	 */
	public ViewPanel(boolean isDoubleBuffered) {
		super(isDoubleBuffered);
		initialize();
	}

	/**
	 * @param layout
	 * @param isDoubleBuffered
	 */
	public ViewPanel(LayoutManager layout, boolean isDoubleBuffered) {
		super(layout, isDoubleBuffered);
		initialize();
	}
	
	private void initialize()
	{
		mShapes =  new java.util.LinkedList<Shape>();
		// mShapes =  new java.util.Vector<Shape>();
		mShapes.add(new Line2D.Double(10, 10, 200, 200));
		mShapes.add(new Line2D.Double(10, 10, 200, 100));

		
		
	}

	@Override protected void paintComponent(Graphics aG)
	{
		Graphics2D fG =(Graphics2D)aG;
		fG.setRenderingHint(RenderingHints.KEY_ANTIALIASING, RenderingHints.VALUE_ANTIALIAS_ON);
		for (Shape fShape : mShapes)
		{
			fG.draw(fShape);
		}
		LineLabel ll = new LineLabel(10, 10, 200, 150);
		ll.draw(fG);
		TextLabel tl = new TextLabel(10, 10, "HelloWorld");
		tl.draw(fG);
	}
	
	java.util.Collection<Shape> mShapes;
}
