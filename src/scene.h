#ifndef SCENE_H
#define SCENE_H

#include <string>
#include <memory>
#include "tinyxml.h"

#include "drawObject.h"
#include "layout.h"
#include "dialogueNode.h"

class Layout;

class Scene {
 public:
    Scene();
    void Init(std::string path, Layout* layout);
    void LoadNodes(TiXmlElement* xmlNode, std::shared_ptr<DialogueNode> parent);
    //static std::vector<std::string> ParseString(std::string input);
    //void PrintNodes(std::shared_ptr<DialogueNode> parent);
    void AdvanceNode();
    static std::string ParseNode(std::shared_ptr<DialogueNode> node);
    void ExecuteCommand(std::string command);
    void SwitchRoot(int index);

    //std::vector<std::shared_ptr<DrawObject>>* m_objects;    // points to vector of pointers to DrawObjects
    Layout* m_layout;
    std::vector<std::shared_ptr<DialogueNode>> m_dialogues; // vector of pointers to DialogueNodes
    std::shared_ptr<DialogueNode>* m_currentRoot; // points to pointer to avoid slicing polymorphic class
    unsigned int m_currentRootIndex;
};

#endif