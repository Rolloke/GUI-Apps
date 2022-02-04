package graphic;


public interface Label {
	/// draw the label
	public abstract void draw(java.awt.Graphics2D aDC);
	/// get bounding rectangle
	public abstract java.awt.geom.Rectangle2D getRect();
	public abstract java.awt.geom.Point2D getPoint(int aP);
	public abstract void setPoint(int aPos, java.awt.geom.Point2D aPoint);
	

}
