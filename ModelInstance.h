#pragma once

#include <memory>

#include "Model.h"

class ModelInstance {
  public:
    std::shared_ptr<const Model> m_model;
};
