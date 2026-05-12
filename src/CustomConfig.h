#ifndef CUSTOMCONFIG_H
#define CUSTOMCONFIG_H

#include <StandardDefines.h>
#include "auth/ISecurityConfig.h"

/* @Configuration */
class CustomConfig : public ISecurityConfig {
    Public CustomConfig() = default;
    Public ~CustomConfig() = default;

    Public Void Configure(IEndpointSecurityRuleManagerPtr endpointSecurityRuleManager) override {
        endpointSecurityRuleManager->AddRule<TestAuthFilter>("/switch/{id}", HttpMethod::GET);
    }
};

#endif // CUSTOMCONFIG_H