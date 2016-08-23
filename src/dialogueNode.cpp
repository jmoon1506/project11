#include "dialogueNode.h"

NodeType DialogueNode::GetNodeType() {
    return m_type;
}

void RootNode::Init() {
    m_type = ROOT;
}

void ResponseNode::Init() {
    m_type = RESPONSE;
}

void TextNode::Init() {
    m_type = TEXT;
}

void CommandNode::Init() {
    m_type = COMMAND;
}

void VariableNode::Init() {
    m_type = VARIABLE;
}

void ConditionNode::Init() {
    m_type = CONDITION;
}

void SubstituteNode::Init() {
    m_type = SUBSTITUTE;
}