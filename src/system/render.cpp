// 通过摄像机来决定渲染内容
void renderSystem(World& world, Camera& cam) {
    glm::mat4 view = getViewMatrix(cam);
    glm::mat4 proj = glm::perspective(
        glm::radians(cam.fov),
        aspect,
        0.1f,
        1000.f
    );

    for (auto& [e, mesh] : world.meshes) {
        auto* t = world.getComponent<Transform>(e);

        glm::mat4 model = glm::translate(glm::mat4(1.0f), t->position);

        shader.set("MVP", proj * view * model);
        draw(mesh);
    }
}
