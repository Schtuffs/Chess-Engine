#include "Move.h"

#include "Piece.h"

Move::Move(INDEX start, INDEX target, FLAG flags) {
    this->m_moveData = (start) | (target << 6) | (flags);
}

Move::~Move() {
    // Nothing todo
}

// Declared functions

INDEX Move::Start() {
    return (this->m_moveData & MASK_MOVE_START);
}

INDEX Move::Target() {
    return ((this->m_moveData & MASK_MOVE_TARGET) >> 6);
}

FLAG Move::Flags() {
    return (this->m_moveData & MASK_MOVE_FLAGS);
}

FLAG Move::Flags(FLAG flag) {
    return (this->m_moveData & flag);
}

void Move::addFlags(FLAG flags) {
    this->m_moveData |= flags;
}

bool Move::isMove() {
    return (this->m_moveData != -1 ? true : false);
}

