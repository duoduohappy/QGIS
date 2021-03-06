/************************************************************************
 * This file has been generated automatically from                      *
 *                                                                      *
 * src/core/geometry/qgstriangle.h                                      *
 *                                                                      *
 * Do not edit manually ! Edit header and run scripts/sipify.pl again   *
 ************************************************************************/




class QgsTriangle : QgsPolygonV2
{
%Docstring
 Triangle geometry type.
.. versionadded:: 3.0
%End

%TypeHeaderCode
#include "qgstriangle.h"
%End
  public:
    QgsTriangle();

    QgsTriangle( const QgsPoint &p1, const QgsPoint &p2, const QgsPoint &p3 );
%Docstring
 Construct a QgsTriangle from three QgsPointV2.
 An empty triangle is returned if there are identical points or if the points are collinear.
 \param p1 first point
 \param p2 second point
 \param p3 third point
%End

    explicit QgsTriangle( const QgsPointXY &p1, const QgsPointXY &p2, const QgsPointXY &p3 );
%Docstring
 Construct a QgsTriangle from three QgsPoint.
 An empty triangle is returned if there are identical points or if the points are collinear.
 \param p1 first point
 \param p2 second point
 \param p3 third point
%End

    explicit QgsTriangle( const QPointF p1, const QPointF p2, const QPointF p3 );
%Docstring
 Construct a QgsTriangle from three QPointF.
 An empty triangle is returned if there are identical points or if the points are collinear.
 \param p1 first point
 \param p2 second point
 \param p3 third point
%End

    bool operator==( const QgsTriangle &other ) const;
    bool operator!=( const QgsTriangle &other ) const;
%Docstring
 :rtype: bool
%End

    virtual QString geometryType() const;
    virtual QgsTriangle *clone() const /Factory/;
    virtual void clear();


    virtual bool fromWkb( QgsConstWkbPtr &wkbPtr );

    virtual bool fromWkt( const QString &wkt );



    virtual QgsPolygonV2 *surfaceToPolygon() const /Factory/;


    virtual QgsAbstractGeometry *toCurveType() const /Factory/;


    virtual void addInteriorRing( QgsCurve *ring /Transfer/ );

%Docstring
Inherited method not used. You cannot add an interior ring into a triangle.
%End

    virtual bool deleteVertex( QgsVertexId position );

%Docstring
Inherited method not used. You cannot delete or insert a vertex directly. Returns always false.
 :rtype: bool
%End
    virtual bool insertVertex( QgsVertexId position, const QgsPoint &vertex );

%Docstring
Inherited method not used. You cannot delete or insert a vertex directly. Returns always false.
 :rtype: bool
%End
    virtual bool moveVertex( QgsVertexId vId, const QgsPoint &newPos );


    virtual void setExteriorRing( QgsCurve *ring /Transfer/ );

    virtual QgsAbstractGeometry *boundary() const /Factory/;


    QgsPoint vertexAt( int atVertex ) const;
%Docstring
  Returns coordinates of a vertex.
  \param atVertex index of the vertex
  :return: Coordinates of the vertex or QgsPoint(0,0) on error (``atVertex`` < 0 or > 3).
 :rtype: QgsPoint
%End

    QVector<double> lengths() const;
%Docstring
 Returns the three lengths of the triangle.
 :return: Lengths of triangle ABC where [AB] is at 0, [BC] is at 1, [CA] is at 2
 * Example:
 \code{.py}
   tri = QgsTriangle( QgsPoint( 0, 0 ), QgsPoint( 0, 5 ), QgsPoint( 5, 5 ) )
   tri.lengths()
   # [5.0, 5.0, 7.0710678118654755]
 \endcode
 :rtype: list of float
%End

    QVector<double> angles() const;
%Docstring
 Returns the three angles of the triangle.
 :return: Angles in radians of triangle ABC where angle BAC is at 0, angle ABC is at 1, angle BCA is at 2
 * Example:
 \code{.py}
   tri = QgsTriangle( QgsPoint( 0, 0 ), QgsPoint( 0, 5 ), QgsPoint( 5, 5 ) )
   [math.degrees(i) for i in tri.angles()]
   # [45.0, 90.0, 45.0]
 \endcode
 :rtype: list of float
%End

    bool isIsocele( double lengthTolerance = 0.0001 ) const;
%Docstring
 Is the triangle isocele (two sides with the same length)?
 \param lengthTolerance The tolerance to use
 :return: True or False
 * Example:
 \code{.py}
   tri = QgsTriangle( QgsPoint( 0, 0 ), QgsPoint( 0, 5 ), QgsPoint( 5, 5 ) )
   tri.lengths()
   # [5.0, 5.0, 7.0710678118654755]
   tri.isIsocele()
   # True
   # length of [AB] == length of [BC]
 \endcode
 :rtype: bool
%End

    bool isEquilateral( double lengthTolerance = 0.0001 ) const;
%Docstring
 Is the triangle equilateral (three sides with the same length)?
 \param lengthTolerance The tolerance to use
 :return: True or False
 * Example:
 \code{.py}
   tri = QgsTriangle( QgsPoint( 10, 10 ), QgsPoint( 16, 10 ), QgsPoint( 13, 15.1962 ) )
   tri.lengths()
   # [6.0, 6.0000412031918575, 6.0000412031918575]
   tri.isEquilateral()
   # True
   # All lengths are close to 6.0
 \endcode
 :rtype: bool
%End

    bool isRight( double angleTolerance = 0.0001 ) const;
%Docstring
 Is the triangle right-angled?
 \param angleTolerance The tolerance to use
 :return: True or False
 * Example:
 \code{.py}
   tri = QgsTriangle( QgsPoint( 0, 0 ), QgsPoint( 0, 5 ), QgsPoint( 5, 5 ) )
   [math.degrees(i) for i in tri.angles()]
   # [45.0, 90.0, 45.0]
   tri.isRight()
   # True
   # angle of ABC == 90
 \endcode
 :rtype: bool
%End

    bool isScalene( double lengthTolerance = 0.0001 ) const;
%Docstring
 Is the triangle scalene (all sides have differen lengths)?
 \param lengthTolerance The tolerance to use
 :return: True or False
 :return: True or False
 * Example:
 \code{.py}
   tri = QgsTriangle( QgsPoint( 7.2825, 4.2368 ), QgsPoint( 13.0058, 3.3218 ), QgsPoint( 9.2145, 6.5242 ) )
   tri.lengths()
   # [5.795980321740233, 4.962793714229921, 2.994131386562721]
   tri.isScalene()
   # True
   # All lengths are different
 \endcode
 :rtype: bool
%End

    QVector<QgsLineString> altitudes() const;
%Docstring
 An altitude is a segment (defined by a QgsLineString) from a vertex to the opposite side (or, if necessary, to the extension of the opposite side).
 :return: Three altitudes from this triangle
 * Example:
 \code{.py}
   tri = QgsTriangle( QgsPoint( 0, 0 ), QgsPoint( 0, 5 ), QgsPoint( 5, 5 ) )
   [alt.asWkt() for alt in tri.altitudes()]
   # ['LineString (0 0, 0 5)', 'LineString (0 5, 2.5 2.5)', 'LineString (5 5, 0 5)']
 \endcode
 :rtype: list of QgsLineString
%End

    QVector<QgsLineString> medians() const;
%Docstring
 A median is a segment (defined by a QgsLineString) from a vertex to the midpoint of the opposite side.
 :return: Three medians from this triangle
 * Example:
 \code{.py}
   tri = QgsTriangle( QgsPoint( 0, 0 ), QgsPoint( 0, 5 ), QgsPoint( 5, 5 ) )
   [med.asWkt() for med in tri.medians()]
   # ['LineString (0 0, 2.5 5)', 'LineString (0 5, 2.5 2.5)', 'LineString (5 5, 0 2.5)']
 \endcode
 :rtype: list of QgsLineString
%End

    QVector<QgsLineString> bisectors( double lengthTolerance = 0.0001 ) const;
%Docstring
 The segment (defined by a QgsLineString) returned bisect the angle of a vertex to the opposite side.
 \param lengthTolerance The tolerance to use
 :return: Three angle bisector from this triangle
 * Example:
 \code{.py}
   tri = QgsTriangle( QgsPoint( 0, 0 ), QgsPoint( 0, 5 ), QgsPoint( 5, 5 ) )
   [bis.asWkt() for bis in tri.bisectors()]
   # ['LineString (0 0, 2.07106781186547462 5)', 'LineString (0 5, 2.5 2.5)', 'LineString (5 5, 0 2.92893218813452538)']
 \endcode
 :rtype: list of QgsLineString
%End

    QgsTriangle medial() const;
%Docstring
 Medial (or midpoint) triangle of a triangle ABC is the triangle with vertices at the midpoints of the triangle's sides.
 :return: The medial from this triangle
 * Example:
 \code{.py}
   tri = QgsTriangle( QgsPoint( 0, 0 ), QgsPoint( 0, 5 ), QgsPoint( 5, 5 ) )
   tri.medial().asWkt()
   # 'Triangle ((0 2.5, 2.5 5, 2.5 2.5, 0 2.5))'
 \endcode
 :rtype: QgsTriangle
%End

    QgsPoint orthocenter( double lengthTolerance = 0.0001 ) const;
%Docstring
 An orthocenter is the point of intersection of the altitudes of a triangle.
 \param lengthTolerance The tolerance to use
 :return: The orthocenter of the triangle.
 * Example:
 \code{.py}
   tri = QgsTriangle( QgsPoint( 0, 0 ), QgsPoint( 0, 5 ), QgsPoint( 5, 5 ) )
   tri.orthocenter().asWkt()
   # 'Point (0 5)'
 \endcode
 :rtype: QgsPoint
%End

    QgsPoint circumscribedCenter() const;
%Docstring
 Center of the circumscribed circle of the triangle.
 :return: The center of the circumscribed circle of the triangle
 * Example:
 \code{.py}
   tri = QgsTriangle( QgsPoint( 0, 0 ), QgsPoint( 0, 5 ), QgsPoint( 5, 5 ) )
   tri.circumscribedCenter().asWkt()
   # 'Point (2.5 2.5)'
 \endcode
 :rtype: QgsPoint
%End

    double circumscribedRadius() const;
%Docstring
 Radius of the circumscribed circle of the triangle.
 :return: The radius of the circumscribed circle of the triangle
 * Example:
 \code{.py}
   tri = QgsTriangle( QgsPoint( 0, 0 ), QgsPoint( 0, 5 ), QgsPoint( 5, 5 ) )
   tri.circumscribedRadius()
   # 3.5355339059327378
 \endcode
 :rtype: float
%End

    QgsCircle circumscribedCircle() const;
%Docstring
 Circumscribed circle of the triangle.
 @return The circumbscribed of the triangle with a QgsCircle.
 Example:
 \code{.py}
   tri = QgsTriangle( QgsPoint( 0, 0 ), QgsPoint( 0, 5 ), QgsPoint( 5, 5 ) )
   tri.circumscribedCircle()
   # QgsCircle(Point (2.5 2.5), 3.5355339059327378, 0)
 \endcode
 :rtype: QgsCircle
%End

    QgsPoint inscribedCenter() const;
%Docstring
 Center of the inscribed circle of the triangle.
 :return: The center of the inscribed circle of the triangle
 * Example:
 \code{.py}
   tri = QgsTriangle( QgsPoint( 0, 0 ), QgsPoint( 0, 5 ), QgsPoint( 5, 5 ) )
   tri.inscribedCenter().asWkt()
   # 'Point (1.46446609406726225 3.53553390593273775)'
 \endcode
 :rtype: QgsPoint
%End

    double inscribedRadius() const;
%Docstring
 Radius of the inscribed circle of the triangle.
 :return: The radius of the inscribed circle of the triangle
 * Example:
 \code{.py}
   tri = QgsTriangle( QgsPoint( 0, 0 ), QgsPoint( 0, 5 ), QgsPoint( 5, 5 ) )
   tri.inscribedRadius()
   # 1.4644660940672622
 \endcode
 :rtype: float
%End

    QgsCircle inscribedCircle() const;
%Docstring
 Inscribed circle of the triangle.
 @return The inscribed of the triangle with a QgsCircle.
 Example:
 \code{.py}
   tri = QgsTriangle( QgsPoint( 0, 0 ), QgsPoint( 0, 5 ), QgsPoint( 5, 5 ) )
   tri.inscribedCircle()
   # QgsCircle(Point (1.46446609406726225 3.53553390593273775), 1.4644660940672622, 0)
 \endcode
 :rtype: QgsCircle
%End

};
/************************************************************************
 * This file has been generated automatically from                      *
 *                                                                      *
 * src/core/geometry/qgstriangle.h                                      *
 *                                                                      *
 * Do not edit manually ! Edit header and run scripts/sipify.pl again   *
 ************************************************************************/
