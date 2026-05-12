#ifndef TEST_AUTH_FILTER_H
#define TEST_AUTH_FILTER_H

#include "auth/IAuthorizationFilter.h"

/**
 * Sample authorization filter: allows access only when the token carries the admin role.
 */
class TestAuthFilter : public IAuthorizationFilter {

    Public Virtual ~TestAuthFilter() override = default;

    Public Virtual std::pair<Bool, optional<ResponseEntity<StdString>>> Authorize(
        const JwtAuthenticationToken& authenticationToken) override {
        if (HasAdminRole(authenticationToken)) {
            return {true, std::nullopt};
        }
        return {false,
                ResponseEntity<StdString>::Unauthorized(
                    StdString("Administrator role required"))};
    }

    Private Static Bool HasAdminRole(const JwtAuthenticationToken& token) {
        for (const auto& authority : token.authorities) {
            if (authority == "ROLE_admin" || authority == "admin") {
                return true;
            }
        }
        const auto roleIt = token.claims.find("role");
        return roleIt != token.claims.end() && roleIt->second == "admin";
    }
};

#endif // TEST_AUTH_FILTER_H
