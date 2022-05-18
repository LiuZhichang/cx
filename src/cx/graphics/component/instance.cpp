#include "instance.h"

#include "cx/common/internal.h"
#include "cx/common/logger.h"
#include "cx/engine/application.h"
#include "cx/engine/engine.h"
#include "cx/window/window.h"

using namespace cx::log;

VKAPI_ATTR VkBool32 VKAPI_CALL debugUtilsMessengerCallback(
    VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
    VkDebugUtilsMessageTypeFlagsEXT messageTypes,
    VkDebugUtilsMessengerCallbackDataEXT const* pCallbackData,
    void* /*pUserData*/) {
#if defined(CX_DEBUG_MODE)
  if (pCallbackData->messageIdNumber == 648835635) {
    // UNASSIGNED-khronos-Validation-debug-build-warning-message
    return VK_FALSE;
  }
  if (pCallbackData->messageIdNumber == 767975156) {
    // UNASSIGNED-BestPractices-vkCreateInstance-specialuse-extension
    return VK_FALSE;
  }
#endif

  LOG_DEBUG(Loggers::engine)
      << vk::to_string(static_cast<vk::DebugUtilsMessageSeverityFlagBitsEXT>(
             messageSeverity))
      << ": "
      << vk::to_string(
             static_cast<vk::DebugUtilsMessageTypeFlagsEXT>(messageTypes))
      << ":\n";
  LOG_DEBUG(Loggers::engine)
      << "\t"
      << "messageIDName   = <" << pCallbackData->pMessageIdName << ">\n";
  LOG_DEBUG(Loggers::engine)
      << "\t"
      << "messageIdNumber = " << pCallbackData->messageIdNumber << "\n";
  LOG_DEBUG(Loggers::engine)
      << "\t"
      << "message         = <" << pCallbackData->pMessage << ">\n";
  if (0 < pCallbackData->queueLabelCount) {
    LOG_DEBUG(Loggers::engine) << "\t"
                               << "Queue Labels:\n";
    for (uint8_t i = 0; i < pCallbackData->queueLabelCount; i++) {
      LOG_DEBUG(Loggers::engine)
          << "\t\t"
          << "labelName = <" << pCallbackData->pQueueLabels[i].pLabelName
          << ">\n";
    }
  }
  if (0 < pCallbackData->cmdBufLabelCount) {
    LOG_DEBUG(Loggers::engine) << "\t"
                               << "CommandBuffer Labels:\n";
    for (uint8_t i = 0; i < pCallbackData->cmdBufLabelCount; i++) {
      LOG_DEBUG(Loggers::engine)
          << "\t\t"
          << "labelName = <" << pCallbackData->pCmdBufLabels[i].pLabelName
          << ">\n";
    }
  }
  if (0 < pCallbackData->objectCount) {
    LOG_DEBUG(Loggers::engine) << "\t"
                               << "Objects:\n";
    for (uint8_t i = 0; i < pCallbackData->objectCount; i++) {
      LOG_DEBUG(Loggers::engine) << "\t\t"
                                 << "Object " << i << "\n";
      LOG_DEBUG(Loggers::engine) << "\t\t\t"
                                 << "objectType   = "
                                 << vk::to_string(static_cast<vk::ObjectType>(
                                        pCallbackData->pObjects[i].objectType))
                                 << "\n";
      LOG_DEBUG(Loggers::engine)
          << "\t\t\t"
          << "objectHandle = " << pCallbackData->pObjects[i].objectHandle
          << "\n";
      if (pCallbackData->pObjects[i].pObjectName) {
        LOG_DEBUG(Loggers::engine)
            << "\t\t\t"
            << "objectName   = <" << pCallbackData->pObjects[i].pObjectName
            << ">\n";
      }
    }
  }
  return VK_TRUE;
}

namespace cx::graphics {

#if defined(CX_DEBUG_MODE)
const std::vector<const char*> Instance::s_validationLayers = {
    "VK_LAYER_KHRONOS_validation"};
#else
const std::vector<const char*> Instance::s_validationLayers = {
    "VK_LAYER_LUNARG_standard_validation"};
#endif

Instance::Instance() : m_instance(VK_NULL_HANDLE) {
  std::vector<const char*> extensions = Window::Get()->get_inst_extensions();
  init(extensions);
}

Instance::Instance(const char** extensions, uint8_t extensions_count)
    : m_instance(VK_NULL_HANDLE) {
  std::vector<const char*> _extensions(extensions,
                                       extensions + extensions_count);
  init(_extensions);
}

Instance::Instance(const std::vector<const char*>& extensions)
    : m_instance(VK_NULL_HANDLE) {
  init(extensions);
}

Instance::~Instance() { m_instance.destroy(); }

void Instance::init(const std::vector<const char*>& extensions) {
  std::string name = "Cx Engine";
  uint32_t engineVersion = Version(1, 1, 1).num();

  vk::ApplicationInfo appInfo(name.c_str(), engineVersion, name.c_str(),
                              engineVersion, VK_API_VERSION_1_1);

  if (is_enable_validations() &&
      !check_validation_layer_support(s_validationLayers)) {
#if defined(CX_DEBUG_MODE)
    LOG_ERROR(Loggers::engine) << "validation layer not enable or invalid!";
#endif
  }

#if defined(CX_DEBUG_MODE)
  vk::StructureChain<vk::InstanceCreateInfo> instanceCreateInfo(
      {{}, &appInfo, s_validationLayers, extensions});
#else
  vk::DebugUtilsMessageSeverityFlagsEXT severityFlags(
      vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning |
      vk::DebugUtilsMessageSeverityFlagBitsEXT::eError);
  vk::DebugUtilsMessageTypeFlagsEXT messageTypeFlags(
      vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral |
      vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance |
      vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation);
  vk::StructureChain<vk::InstanceCreateInfo,
                     vk::DebugUtilsMessengerCreateInfoEXT>
      instanceCreateInfo(
          {{}, &appInfo, s_validationLayers, extensions},
          {{}, severityFlags, messageTypeFlags, &debugUtilsMessengerCallback});
#endif
  m_instance =
      vk::createInstance(instanceCreateInfo.get<vk::InstanceCreateInfo>());

#if defined(CX_DEBUG_MODE)
  LOG_INFO(Loggers::engine) << "The vulkan instance is created successfully.";
#endif
}

bool Instance::is_enable_validations() const {
#if defined(CX_DEBUG_MODE)
  return true;
#else
  return false;
#endif
}

uint32_t Instance::FindMemoryTypeIndex(
    vk::PhysicalDeviceMemoryProperties const& memoryProperties,
    uint32_t typeBits, vk::MemoryPropertyFlags requirementsMask) {
  uint32_t typeIndex = uint32_t(~0);
  for (uint32_t i = 0; i < memoryProperties.memoryTypeCount; i++) {
    if ((typeBits & 1) && ((memoryProperties.memoryTypes[i].propertyFlags &
                            requirementsMask) == requirementsMask)) {
      typeIndex = i;
      break;
    }
    typeBits >>= 1;
  }
  assert(typeIndex != uint32_t(~0));
  return typeIndex;
}

bool Instance::check_validation_layer_support(
    const std::vector<const char*>& layers) {
  std::vector<vk::LayerProperties> layerProps =
      vk::enumerateInstanceLayerProperties();

  for (const char* layer : layers) {
    bool found = false;
    for (const vk::LayerProperties& prop : layerProps) {
      if (::strcmp(prop.layerName.data(), layer) == 0) {
        found = true;
        break;
      }
    }

    if (!found) {
#if defined(CX_MODEL_DEBUG)
      LOG_ERROR(logger) << "not support layer:" << layer << ", not found!";
#endif
      return false;
    }
  }

  return true;
}
}  // namespace cx::graphics