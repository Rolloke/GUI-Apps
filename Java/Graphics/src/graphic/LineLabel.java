package graphic;

import java.awt.Graphics2D;
import java.awt.geom.Line2D;
import java.awt.geom.Point2D;
import java.awt.geom.Rectangle2D;

public class LineLabel implements Label{

	LineLabel(double x1, double y1, double x2, double y2){
		mLine = new Line2D.Double(x1, y1, x2, y2);
	}
	@Override
	public void draw(Graphics2D aDC) {
		aDC.draw(mLine); 
	}

	@Override
	public Point2D getPoint(int aP) {
		if (aP == 0) return mLine.getP1();
		return mLine.getP2();
	}

	@Override
	public Rectangle2D getRect() {
		return new Rectangle2D.Double(mLine.getX1(), mLine.getY1(), mLine.getX2(), mLine.getY2());
	}

	@Override
	public void setPoint(int aPos, Point2D aPoint) {
		if (aPos == 0) mLine.setLine(aPoint, mLine.getP2());
		else           mLine.setLine(mLine.getP1(), aPoint);
	}

	private java.awt.geom.Line2D mLine;

}
