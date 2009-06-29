#pragma once

#include "Range.h"

namespace P3D
{
    /*
    Default operations over path point.
    Implement your explicit template to override.
    */
    template<class T>
    struct PathPointTypeTraits
    {
        static T Multiply(Scalar t, const T& val)
        {
            return t * val;
        }

        static T Add(const T& val1, const T& val2)
        {
            return val1 + val2;
        }
    };

    /*
    Traits for QTransform.
    */
    template<>
    struct PathPointTypeTraits<QTransform>
    {
        static QTransform Multiply(Scalar t, const QTransform& val)
        {
            return QTransform(val.Translation * t, Quaternion(
                              val.Rotation.x * t, 
                              val.Rotation.y * t, 
                              val.Rotation.z * t,
                              val.Rotation.w * t));
        }

        static QTransform Add(const QTransform& val1, const QTransform& val2)
        {
            return QTransform(val1.Translation + val2.Translation, Quaternion(
                              val1.Rotation.x + val2.Rotation.x,
                              val1.Rotation.y + val2.Rotation.y,
                              val1.Rotation.z + val2.Rotation.z,
                              val1.Rotation.w + val2.Rotation.w));
        }
    };

    /*
    A set of elements (vectors, quaternions, whatever) with time values.
    */
    template<class T>
    class Path : public Object
    {
        typedef typename PathPointTypeTraits<T> Traits;

    public:
        mathinline Path()
        { 
            MinT = MaxT = 0;
        }

        /*
        Add point to the list.
        */
        mathinline void AppendPoint(Scalar time, const T& position)
        {
            if (time < MinT) MinT = time;
            if (time > MaxT) MaxT = time;

            PointsList::iterator it = Points.end();
            while (it != Points.begin())
            {
                --it;
                if (time >= it->Time)
                {
                    Points.insert(++it, Point(position, time));
                    //PointAdded(this, time, position);
                    return;
                }
            }
            Points.push_back(Point(position, time));
            //PointAdded(this, time, position);
        }

        /*
        Clears the path.
        */
        mathinline void Clear()
        {
            MinT = MaxT = 0;
            Points.clear();
        }

        /*
        Returns interpolated path position.
        */
        mathinline void Interpolate(Scalar time, T& outputPos) const
        {
            if (Points.size() == 0) return;
            if (Points.size() == 1)
            {
                outputPos = Points[0].Position;
                return;
            }

            if (time <= Points[0].Time)
            {
                outputPos = Points[0].Position;
                return;
            }

            if (time >= Points.back().Time)
            {
                outputPos = Points.back().Position;
                return;
            }

            int left = 0, right = Points.size();
            while ((right - left) != 1)
            {
                int middle = (left + right) / 2;
                Scalar middleVal = Points[middle].Time;
                if (time >= middleVal)
                    left = middle;
                else
                    right = middle;
            }

            if (Points[left].Time == Points[right].Time) return;

            Scalar t = (time - Points[left].Time) / (Points[right].Time - Points[left].Time);

            outputPos = Traits::Add(Traits::Multiply((1.0f - t), Points[left].Position),
                                    Traits::Multiply(t, Points[right].Position));
        }

        /*
        Called when point added to the Path.
        */
        //signal3<const Path<T>*, Scalar, const T&> PointAdded;

    public:
        /*
        One path element.
        */
        struct Point
        {
            Point() 
            {}

            Point(const T& position, Scalar time)
                : Position(position), Time(time)
            { }

            Scalar Time;
            T Position;
        };

        typedef std::vector<Point> PointsList;

        /*
        List of all path points.
        */
        PointsList Points;

        /*
        Minimal value of the parameter.
        */
        Scalar MinT;

        /*
        Maximal value of the parameter.
        */
        Scalar MaxT;
    };
}