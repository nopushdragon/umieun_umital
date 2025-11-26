#include "collision.h"

bool is_separated(const OBB& a, const OBB& b, const glm::vec3& axis) {
    if (glm::length(axis) < 1e-6) return false;

    glm::vec3 T = b.center - a.center;

    float distance_proj = glm::abs(glm::dot(T, axis));

    float radius_a =
        glm::abs(glm::dot(a.half_length.x * a.u[0], axis)) +
        glm::abs(glm::dot(a.half_length.y * a.u[1], axis)) +
        glm::abs(glm::dot(a.half_length.z * a.u[2], axis));

    float radius_b =
        glm::abs(glm::dot(b.half_length.x * b.u[0], axis)) +
        glm::abs(glm::dot(b.half_length.y * b.u[1], axis)) +
        glm::abs(glm::dot(b.half_length.z * b.u[2], axis));

    return distance_proj > (radius_a + radius_b);
}

bool check_collision(const OBB& obbA, const OBB& obbB) {
    const OBB& a = obbA;
    const OBB& b = obbB;

    for (int i = 0; i < 3; i++) {
        if (is_separated(a, b, a.u[i])) return false;
    }

    for (int i = 0; i < 3; i++) {
        if (is_separated(a, b, b.u[i])) return false;
    }

    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            glm::vec3 cross_axis = glm::cross(a.u[i], b.u[j]);
            if (is_separated(a, b, cross_axis)) return false;
        }
    }

    return true;
}

void drawDebugOBB(GLuint shaderID, const OBB& obb, const glm::mat4& view, const glm::mat4& proj, const glm::vec3& color) {
    if (shaderID == 0) return;

    glUseProgram(shaderID);

    // 1. 필수 공통 유니폼 설정 (이동, 카메라 위치, 조명 위치 등)
    glUniformMatrix4fv(glGetUniformLocation(shaderID, "uView"), 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(glGetUniformLocation(shaderID, "uProj"), 1, GL_FALSE, glm::value_ptr(proj));
    glUniform3fv(glGetUniformLocation(shaderID, "viewPos"), 1, glm::value_ptr(view[0]));

    GLint originalBUseTexture;
    glm::vec3 originalMaterialColorDefault;
    float originalAmbientStrength;
    int originalShininess;
    glm::vec3 originalMaterialSpecular;

    // A. 현재 상태 저장 (이 값들을 복원해야 함)
    glGetUniformiv(shaderID, glGetUniformLocation(shaderID, "bUseTexture"), &originalBUseTexture);
    glGetUniformfv(shaderID, glGetUniformLocation(shaderID, "materialColorDefault"), glm::value_ptr(originalMaterialColorDefault));
    glGetUniformfv(shaderID, glGetUniformLocation(shaderID, "ambientStrength"), &originalAmbientStrength);
    glGetUniformiv(shaderID, glGetUniformLocation(shaderID, "shininess"), &originalShininess);
    glGetUniformfv(shaderID, glGetUniformLocation(shaderID, "materialSpecular"), glm::value_ptr(originalMaterialSpecular));

    // B. 조명 무시를 위한 값 강제 설정
    glUniform1i(glGetUniformLocation(shaderID, "bUseTexture"), 0); // 텍스처 비활성화 [cite: 2, 5]
    glUniform3fv(glGetUniformLocation(shaderID, "materialColorDefault"), 1, glm::value_ptr(color)); // 기본 색상으로 디버그 색상 주입 [cite: 2, 7]
    glUniform1f(glGetUniformLocation(shaderID, "ambientStrength"), 1.0f); // 앰비언트 최대화 (조명 색상 * 기본 색상) [cite: 3, 7]
    glUniform1i(glGetUniformLocation(shaderID, "shininess"), 1); // pow(..., 1)이 되어 specular가 조명 강도에만 영향을 받게 하거나, 
    // 0으로 설정하여 아예 거울 반사를 없앨 수 있습니다.
    // 여기서는 1로 두어 스페큘러 항을 최소화하고, materialSpecular를 0으로 설정합니다. [cite: 3, 11]
    glUniform3fv(glGetUniformLocation(shaderID, "materialSpecular"), 1, glm::value_ptr(glm::vec3(0.0f))); // 스페큘러 기여도 0으로 설정 [cite: 3, 12]

    // ----------------------------------------------------
    // OBB 속성을 이용한 8개 정점 직접 계산
    glm::vec3 E_x = obb.u[0] * obb.half_length.x; // X축 길이 * 방향
    glm::vec3 E_y = obb.u[1] * obb.half_length.y; // Y축 길이 * 방향
    glm::vec3 E_z = obb.u[2] * obb.half_length.z; // Z축 길이 * 방향

    // OBB의 8개 꼭짓점을 월드 좌표로 계산하여 저장
    glm::vec3 vertices[] = {
        // [1] Z+ 면 (앞면)
        obb.center + E_x + E_y + E_z,  // 0: +X +Y +Z
        obb.center - E_x + E_y + E_z,  // 1: -X +Y +Z
        obb.center + E_x - E_y + E_z,  // 2: +X -Y +Z
        obb.center - E_x - E_y + E_z,  // 3: -X -Y +Z

        // [2] Z- 면 (뒷면)
        obb.center + E_x + E_y - E_z,  // 4: +X +Y -Z
        obb.center - E_x + E_y - E_z,  // 5: -X +Y -Z
        obb.center + E_x - E_y - E_z,  // 6: +X -Y -Z
        obb.center - E_x - E_y - E_z   // 7: -X -Y -Z
    };

    // 12개의 선분을 그리기 위한 24개의 인덱스 (순서는 유지)
    unsigned int indices[] = {
        1, 0, 0, 2, 2, 3, 3, 1, // 앞면 (Z+)
        5, 4, 4, 6, 6, 7, 7, 5, // 뒷면 (Z-)
        1, 5, 0, 4, 2, 6, 3, 7  // 기둥
    };

    GLuint obbVAO, obbVBO, obbEBO;
    glGenVertexArrays(1, &obbVAO);
    glGenBuffers(1, &obbVBO);
    glGenBuffers(1, &obbEBO);

    // --- VAO 설정 ---
    glBindVertexArray(obbVAO);

    glBindBuffer(GL_ARRAY_BUFFER, obbVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, obbEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // 정점 속성 포인터 설정 (기존 셰이더의 aPos 위치 0 사용)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
    glEnableVertexAttribArray(0);
    glDisableVertexAttribArray(1); // aNormal (Location 1) 
    glDisableVertexAttribArray(2); // aTexCoords (Location 2)

    // ----------------------------------------------------
    // Model Matrix 구성 및 드로우

    // OBB의 정보를 기반으로 Model Matrix를 구성 (Center + Orientation + Scale)
    glm::mat4 model = glm::mat4(1.0f);

    //model[0] = glm::vec4(obb.u[0] * obb.half_length.x, 0.0f); // X축 (u[0])
    //model[1] = glm::vec4(obb.u[1] * obb.half_length.y, 0.0f); // Y축 (u[1])
    //model[2] = glm::vec4(obb.u[2] * obb.half_length.z, 0.0f); // Z축 (u[2])
    //model[3] = glm::vec4(obb.center, 1.0f);

    glUniformMatrix4fv(glGetUniformLocation(shaderID, "uModel"), 1, GL_FALSE, glm::value_ptr(model));

    // 드로우 호출 (GL_LINES)
    glBindVertexArray(obbVAO);
    glDrawElements(GL_LINES, 24, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    // --- 리소스 해제 ---
    glDeleteBuffers(1, &obbVBO);
    glDeleteBuffers(1, &obbEBO);
    glDeleteVertexArrays(1, &obbVAO);

    // 환경 복원
    glUniform1i(glGetUniformLocation(shaderID, "bUseTexture"), originalBUseTexture);
    glUniform3fv(glGetUniformLocation(shaderID, "materialColorDefault"), 1, glm::value_ptr(originalMaterialColorDefault));
    glUniform1f(glGetUniformLocation(shaderID, "ambientStrength"), originalAmbientStrength);
    glUniform1i(glGetUniformLocation(shaderID, "shininess"), originalShininess);
    glUniform3fv(glGetUniformLocation(shaderID, "materialSpecular"), 1, glm::value_ptr(originalMaterialSpecular));
}