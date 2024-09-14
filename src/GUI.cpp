#include "GUI.hpp"

#include <imgui.h>
#include <imgui-SFML.h>

#include <utility>

#include "Tags.hpp"

GUI::GUI(Assets& assets, EntityManager& entityManager):
    m_assets(assets), m_entityManager(entityManager) {};

void GUI::showDebugWindow(bool& m_drawGrid, bool& m_drawTextures, bool& m_drawCollision, bool& m_zoom)
{
    ImGui::Begin("Scene Properties");

    if (ImGui::BeginTabBar("DebugBar"))
    {
        if (ImGui::BeginTabItem("Debug"))
        {
            ImGui::Checkbox("Draw Grid (G)", &m_drawGrid);
            ImGui::Checkbox("Draw Textures (T)", &m_drawTextures);
            ImGui::Checkbox("Draw Debug (C)", &m_drawCollision);
            ImGui::Checkbox("Zoom Cam (Z)", &m_zoom);

            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("Sounds"))
        {
            for (auto& [name, sound]: m_assets.getSounds())
            {
                ImGui::PushID(name.c_str()); // bind id with a unique identifier which is a name
                if (ImGui::Button("Play")) { sound.play(); }
                ImGui::PopID(); // unbind an id
                ImGui::SameLine();

                ImGui::PushID(name.c_str());
                if (ImGui::Button("Stop")) { sound.stop(); }
                ImGui::PopID();
                ImGui::SameLine();

                ImGui::Text("%s", name.c_str()); // Sounds name
            }

            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("Entity Manager"))
        {
            if (ImGui::TreeNode("Entities by Tag"))
            {
                for (auto& [tag, entityVector]: m_entityManager.getEntityMap())
                {
                    const bool showHeader = (tag != TagName::ePlayer);
                    if (ImGui::CollapsingHeader(tags[tag].c_str()))
                    {
                        guiShowTable(entityVector, showHeader);
                    }
                }
                ImGui::TreePop();
            }

            if (ImGui::TreeNode("All Entities"))
            {
                guiShowTable(m_entityManager.getEntities());

                ImGui::TreePop();
            }

            ImGui::EndTabItem();
        }
        ImGui::EndTabBar();
    }
    ImGui::End();
}

void GUI::guiShowTable(const std::vector<Entity>& entities, bool showHeader)
{
    if (ImGui::BeginTable("Table", 7))
    {
        ImGui::TableSetupColumn("Del", ImGuiTableColumnFlags_WidthFixed, 30.0f);
        ImGui::TableSetupColumn("Img", ImGuiTableColumnFlags_WidthFixed, 40.0f);
        ImGui::TableSetupColumn("ID", ImGuiTableColumnFlags_WidthFixed, 30.0f);
        ImGui::TableSetupColumn("Type", ImGuiTableColumnFlags_WidthFixed, 45.0f);
        ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthFixed, 90.0f);
        ImGui::TableSetupColumn("Status", ImGuiTableColumnFlags_WidthFixed, 40.0f);
        ImGui::TableSetupColumn("Position");

        if (showHeader) { ImGui::TableHeadersRow(); }

        for (auto entity: entities)
        {
            auto& anim = entity.get<CAnimation>().animation;
            auto& pos = entity.get<CTransform>().pos;

            ImGui::TableNextRow(0, 32.0f);

            ImGui::TableSetColumnIndex(0);
            if (ImGui::Button(("[x]##%" + std::to_string(entity.id())).c_str()))
            {
                entity.destroy();
                // destroyEntity(entity);
                if (entity.tag() == "Player") { m_callback(); }
            }
            ImGui::SetItemTooltip("Remove %s", entity.tag().c_str());

            ImGui::TableSetColumnIndex(1);
            ImGui::Image(anim.getSprite(), sf::Vector2f(30.0f, 30.0f));

            ImGui::TableSetColumnIndex(2);
            ImGui::Text("%d", static_cast<int>(entity.id()));

            ImGui::TableSetColumnIndex(3);
            ImGui::Text("%s", entity.tag().c_str());

            ImGui::TableSetColumnIndex(4);
            ImGui::Text("%s", anim.getName().c_str());

            ImGui::TableSetColumnIndex(5);
            ImGui::Text("%s", (entity.isActive() ? "true" : "false"));

            ImGui::TableSetColumnIndex(6);
            ImGui::Text("(%d, %d)", static_cast<int>(pos.x), static_cast<int>(pos.y));
        }

        ImGui::EndTable();
    }
}

void GUI::addCallBack(std::function<void()> callback)
{
    m_callback = std::move(callback);
}
