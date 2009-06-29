#pragma once

namespace P3D
{
    namespace Physics
    {
        /*
        Collision model base class.
        */
        class CollisionModel : 
            public Object
        {
        public:
            enum ModelType
            {
                Box
            };

        protected:
            CollisionModel(ModelType type) : _type(type) {}
            virtual ~CollisionModel() {}

        public:
            ModelType GetType() const { return _type; }

        private:
            ModelType _type;
        };
    }
}