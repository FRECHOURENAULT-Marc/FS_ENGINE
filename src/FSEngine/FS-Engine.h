#pragma once

//Framework
#include "../FSRender/FS-Render.h"
using namespace FSR;

namespace FSE {};

//Our tools
#include "utils.h"
#include "debug.h"
//Other tools
#include <type_traits>
#include <stdio.h>
#include <stdlib.h>
#include <ctime>
#include <algorithm>

//Render Files
#include "Buffer.h"

#include "Object.h"
#include "Entity.h"

#include "RayCast.h"

#include "Components/Component.h"
#include "Components/TransformComponent.h"
#include "Components/MeshComponent.h"
#include "Components/VelocityComponent.h"
#include "Components/StateMachineComponent.h"
#include "Components/EmitterComponent.h"
#include "Components/CameraComponent.h"
#include "Components/ColliderComponent.h"
#include "Components/LightComponent.h"
#include "Components/SpriteComponent.h"

#include "Systems/System.h"
#include "Systems/MoveSystem.h"
#include "Systems/MeshWorldSystem.h"
#include "Systems/StateMachineSystem.h"
#include "Systems/EmitterSystem.h"
#include "Systems/CameraSystem.h"
#include "Systems/ColliderSystem.h"
#include "Systems/LightSystem.h"
#include "Systems/TransformSystem.h"
#include "Systems/SpriteSystem.h"

#include "Scripts/Script.h"

#include "SceneManager.h"
#include "ECS.h"

using namespace FSE;