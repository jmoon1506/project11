#ifndef DIALOGUEOUTPUT_H
#define DIALOGUEOUTPUT_H

class OutputRecord() {
 public:
    std::string m_text;
}

class OutputResponse() {
 public:
    std::string m_text;
    std::shared_ptr<DialogueNode>* m_node;
}

#endif