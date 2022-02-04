package graphic;

import java.awt.Font;
import java.awt.Graphics2D;
import java.awt.Rectangle;
import java.awt.font.FontRenderContext;
import java.awt.geom.AffineTransform;
import java.awt.geom.Point2D;
import java.awt.geom.Rectangle2D;

public class TextLabel implements Label {

	TextLabel(double x, double y, String aText) {
		mPosition = new Point2D.Double(x, y);
		mText = aText;
	}
	@Override public void draw(Graphics2D aDC) {
		aDC.setFont(mFont);
		aDC.drawString(mText, (float)mPosition.getX(), (float)mPosition.getY());
	}

	@Override public Point2D getPoint(int aP) {
		return mPosition;
	}

	@Override public Rectangle2D getRect() {

		FontRenderContext  frc = new FontRenderContext(null, false, false);
		return mFont.getStringBounds(mText, frc);
	}

	@Override public void setPoint(int aPos, Point2D aPoint) {
		mPosition = aPoint;
	}

	Point2D mPosition;
	String  mText;
	Font    mFont;
}
