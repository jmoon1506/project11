#include <string>
//#include <boost/tokenizer.hpp>
#include <vector>
#include <typeinfo>
#include <stdlib.h>
#define TIXML_USE_STL 
#include "tinyxml.h"
//#include "rapidxml/rapidxml.hpp"
//#include "yaml-cpp/yaml.h"

#include "layout.h"
#include "scene.h"
#include "dialogueNode.h"

//typedef boost::tokenizer<boost::char_separator<char>> tokenizer;

Scene::Scene() {
}

void Scene::Init(std::string path, Layout* layout) {
    m_layout = layout;
    //const char* char_path = path.c_str();
    TiXmlDocument doc(path);
    if(!doc.LoadFile()) {
        printf("XML at %s failed to load", path.c_str());
    }
    TiXmlElement* scene = doc.FirstChildElement("scene"); 


    for (TiXmlElement* node = scene->FirstChildElement(); node != NULL; node = node->NextSiblingElement()) {
        std::string rootType = node->Value();
        if (rootType == "flags") {
            for (TiXmlElement* flag = node->FirstChildElement(); flag != NULL; flag = flag->NextSiblingElement()) {
                //printf("%s: %s\n", flag->Value(), flag->GetText());
            }
        } else if (rootType == "dialogue") {
            auto dialogue = std::shared_ptr<DialogueNode>(new RootNode());
            LoadNodes(node, dialogue);
            m_dialogues.push_back(dialogue);
        }
    }
    m_currentRoot = &m_dialogues.front();
    m_currentRootIndex = 0;
    //PrintNodes(*currentNode);
    /*
    for (auto it = m_dialogues.begin(); it != m_dialogues.end(); ++it) {
        PrintNodes((*it));
    }
    */
}
/*
void Scene::PrintNodes(std::shared_ptr<DialogueNode> parent) {
    if (parent->m_type != SUBSTITUTE)
        std::cout << parent->m_contents << "\n";
    for (auto it = parent->m_children.begin(); it != parent->m_children.end(); ++it) {
        PrintNodes((*it));
    }
}
*/
void Scene::SwitchRoot(int index) {

}

void Scene::LoadNodes(TiXmlElement* xmlNode, std::shared_ptr<DialogueNode> parent) {
    for (TiXmlElement* child = xmlNode->FirstChildElement(); child != NULL; child = child->NextSiblingElement()) {
        std::string xmlType = child->Value();
        if (xmlType == "prob") {
            parent->m_prob = static_cast<float>(atof(child->GetText()));
            continue;
        } else if (xmlType == "contents") {
            if (child->GetText() != NULL)
                parent->m_contents = child->GetText();
            continue;
        } else if (xmlType == "index") {
            parent->m_index = atoi(child->GetText());
            continue;
        }

        std::shared_ptr<DialogueNode> node;
        if (xmlType == "response") {
            node = std::shared_ptr<DialogueNode>(new ResponseNode());
        } else if (xmlType == "text") {
            node = std::shared_ptr<DialogueNode>(new TextNode());
        } else if (xmlType == "command") {
            node = std::shared_ptr<DialogueNode>(new CommandNode());
        } else if (xmlType == "variable") {
            node = std::shared_ptr<DialogueNode>(new VariableNode());
        } else if (xmlType == "substitute") {
            node = std::shared_ptr<DialogueNode>(new SubstituteNode());
        } else if (xmlType == "condition") {
            node = std::shared_ptr<DialogueNode>(new ConditionNode());
        }
        node->Init();
        LoadNodes(child, node);
        parent->m_children.push_back(node);
    }
}

void Scene::ExecuteCommand(std::string command) {

}

void Scene::AdvanceNode() {
    if (m_currentRootIndex < (*m_currentRoot)->m_children.size()) {
        std::shared_ptr<DialogueNode> &currentNode = (*m_currentRoot)->m_children[m_currentRootIndex];
        switch (currentNode->m_type) {
            case RESPONSE:
                for (std::size_t i = m_currentRootIndex; i < (*m_currentRoot)->m_children.size(); ++i) {
                    // add dialogue response to layout
                }
                break;
            case TEXT:
                // add dialogue text to layout
                m_layout->AddText(currentNode->m_contents);
                break;
            case COMMAND:
                // execute command
                break;
            default:
                break;
        }
        m_currentRootIndex++;
        //m_outputs.push_back(new OutputRecord());
    }
}

// Fill in substitutes
std::string Scene::ParseNode(std::shared_ptr<DialogueNode> node) {
    std::string output = "";
    for (auto it = node->m_children.begin(); it != node->m_children.end(); ++it) {
        ParseNode((*it));
    }
    return output;
}

/*
void Scene::Load(std::string path, std::vector<std::shared_ptr<DrawObject>>* objects) {
    printf("Load!\n");
    m_objects = objects;
    YAML::Node scene = YAML::LoadFile(path)["scene"];
    for (auto it=scene.begin(); it != scene.end(); ++it) {
        YAML::Node children = it->second;
        std::string rootType = it->first.as<std::string>();
        if (rootType == "flags") {
            for (auto it2 = children.begin(); it2 != children.end(); ++it2) {
                printf("%s: %s\n", it2->first.as<std::string>().c_str(), it2->second.as<std::string>().c_str());
            }
        }
        else if (rootType == "dialogue") {
            auto dialogue = std::shared_ptr<DialogueNode>(new RootNode());
            LoadNodes(children, dialogue);
            m_dialogues.push_back(dialogue);
        }
    }
}

void Scene::LoadNodes(YAML::Node& yamlNode, std::shared_ptr<DialogueNode> parent) {
    for (auto it=yamlNode.begin(); it != yamlNode.end(); ++it) {
        
        std::string yamlType = it->first.as<std::string>();
        if (yamlType == "prob") {
            parent->m_prob = it->second.as<float>();
            continue;
        } else if (yamlType == "content") {
            parent->m_contents = it->second.as<std::string>();
            printf("%s: %s\n", typeid(*parent).name(), it->second.as<std::string>().c_str());
            continue;
        } else if (yamlType == "index") {
            parent->m_index = it->second.as<int>();
            continue;
        }

        std::shared_ptr<DialogueNode> node;
        if (yamlType == "response") {
            node = std::shared_ptr<DialogueNode>(new ResponseNode());
        } else if (yamlType == "text") {
            node = std::shared_ptr<DialogueNode>(new TextNode());
        } else if (yamlType == "command") {
            node = std::shared_ptr<DialogueNode>(new CommandNode());
        } else if (yamlType == "variable") {
            node = std::shared_ptr<DialogueNode>(new VariableNode());
        } else if (yamlType == "substitute") {
            node = std::shared_ptr<DialogueNode>(new SubstituteNode());
        } else if (yamlType == "condition") {
            node = std::shared_ptr<DialogueNode>(new ConditionNode());
        }

        YAML::Node children = it->second;
        if (children.size() == 0) {
            node->m_contents = it->second.as<std::string>();
            printf("%s: %s\n", it->first.as<std::string>().c_str(), it->second.as<std::string>().c_str());
        } else {
            LoadNodes(children, node);
        }
        parent->m_children.push_back(node);
    }
}

std::vector<std::string> Scene::ParseString(std::string input) {
    std::vector<std::string> strings;
    boost::char_separator<char> sep(" ", "!<>=");
    tokenizer tokens(input, sep);
    for (tokenizer::iterator tok_iter = tokens.begin(); tok_iter != tokens.end(); ++tok_iter) {
        strings.push_back(*tok_iter);
    }
    return strings;
}
*/