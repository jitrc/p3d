#include "Includes.h"
#include "BulletController.h"
#include "BulletMathIterop.h"

namespace P3D
{
    namespace Physics
    {
        Logger BulletController::logger(L"Physics.BulletController");

        BulletController::BulletController(BulletWorld* world, World::Entity* entity)
        {
            _body = NULL;
            _entity = entity;
        }

        void BulletController::Bind(World::Entity* entity)
        {
            ASSERT(_body == NULL);
            ASSERT(_entity == entity);
        }

        void BulletController::Unbind(World::Entity* entity)
        {
            if (_body != NULL)
            {
                delete _body; 
                _body = NULL;
            }
        }

        void BulletController::UpdateEntity(World::Entity* entity, const TimeCounter& time)
        {
            ASSERT(_body != NULL);
            _entity->SetTransformToWorldSpace(Conv(_toWorldSpace));
        }

        void BulletController::Prepare(World::Entity* entity)
        {
            ASSERT(entity->GetCollisionModel() != NULL);
            CollisionModel* cm = entity->GetCollisionModel();
            btCollisionShape* shape = NULL;
            switch (cm->GetType())
            {
                case CollisionModel::Box:
                    shape = ((BulletBoxCollisionModel*)cm);
                break;
                default:
                    shape = NULL;
            }
            ASSERT(shape != NULL);

            btVector3 inertia(0, 0, 0);
            shape->calculateLocalInertia(entity->GetMass(), inertia);

            btRigidBody::btRigidBodyConstructionInfo ci(
                entity->GetMass(),  this, shape, inertia);
            _body = new btRigidBody(ci);

            ((BulletWorld*)entity->GetWorld()->GetPhysicalWorld())->GetDynamicsWorld()->addRigidBody(_body);
        }

        // From btMotionState
        void BulletController::getWorldTransform(btTransform& worldTrans) const
        {
            ASSERT(_entity != NULL);
            const QTransform& transform = _entity->GetTransformToWorldSpace();
            worldTrans = Conv(transform);
            _toWorldSpace = worldTrans;
        }

        //Bullet only calls the update of worldtransform for active objects
        void BulletController::setWorldTransform(const btTransform& worldTrans)
        {
            _toWorldSpace = worldTrans;
        }
    }
}