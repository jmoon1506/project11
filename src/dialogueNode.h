#ifndef DIALOGUENODE_H
#define DIALOGUENODE_H

#include <vector>
#include <memory>

typedef enum {
    ROOT = 0,
    RESPONSE,
    TEXT,
    COMMAND,
    VARIABLE,
    CONDITION,
    SUBSTITUTE
} NodeType;

typedef enum {
    LT = 0,
    LTE,
    EQ,
    NEQ,
    GTE,
    GT
} LogicalOperator;

typedef union dataValue {
    bool v_bool;
    int v_int;
    float v_float;
    std::string v_string;
    ~dataValue() {}
} DataValue;

typedef struct {
    std::string var;
    LogicalOperator op;
    DataValue data;
} Condition;

typedef struct {
    std::string var;
    DataValue data;
} Command;

typedef struct {
    std::string var;
    DataValue data;
} Flag;

class DialogueNode {
 public:
    NodeType GetNodeType();
    //virtual NodeType GetNodeType() = 0;
    virtual void Init() = 0;

    NodeType m_type;
    std::string m_contents;
    int m_index;
    float m_prob;
    std::vector<Condition> m_conditions;
    std::vector<std::shared_ptr<DialogueNode>> m_children;
};

class RootNode : public DialogueNode {
 public:
    void Init();
};

class ResponseNode : public DialogueNode {
 public:
    void Init();
};

class TextNode : public DialogueNode {
 public:
    void Init();
};

class CommandNode : public DialogueNode {
 public:
    void Init();
};

class VariableNode : public DialogueNode {
 public:
    void Init();
};

class ConditionNode : public DialogueNode {
 public:
    void Init();
};

class SubstituteNode : public DialogueNode {
 public:
    void Init();
};

/*
class Dialogue {
 public:
    Dialogue();
    void Add(DialogueNode node);
    std::vector<std::shared_ptr<DialogueNode>> nodes;
};
typedef union DataValue {
    int v_int;
    float v_float;
    char* v_char;
}
*/
#endif