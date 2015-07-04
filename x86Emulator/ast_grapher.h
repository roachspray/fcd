//
//  ast_grapher.hpp
//  x86Emulator
//
//  Created by Félix on 2015-07-03.
//  Copyright © 2015 Félix Cloutier. All rights reserved.
//

#ifndef ast_grapher_cpp
#define ast_grapher_cpp

#include "ast_nodes.h"
#include "dumb_allocator.h"

SILENCE_LLVM_WARNINGS_BEGIN()
#include <llvm/IR/CFG.h>
SILENCE_LLVM_WARNINGS_END()

#include <unordered_map>
#include <deque>

class AstGrapher;

class AstGraphNode
{
	friend class llvm::GraphTraits<AstGraphNode>;
	AstGrapher& grapher;
	
public:
	Statement* node;
	llvm::BasicBlock* entry;
	llvm::BasicBlock* exit;
	
	AstGraphNode(AstGrapher& grapher, Statement* node, llvm::BasicBlock* entry, llvm::BasicBlock* exit);
};

class AstGrapher
{
	DumbAllocator<>& pool;
	std::deque<AstGraphNode> nodeStorage;
	std::unordered_map<llvm::BasicBlock*, Statement*> nodeByEntry;
	std::unordered_map<Statement*, AstGraphNode*> graphNodeByAstNode;
	
public:
	typedef decltype(nodeStorage)::iterator nodes_iterator;
	
	explicit AstGrapher(DumbAllocator<>& pool);
	
	Statement* addBasicBlock(llvm::BasicBlock& bb);
	void updateRegion(llvm::BasicBlock& entry, llvm::BasicBlock& exit, Statement& node);
	
	AstGraphNode* getGraphNode(llvm::BasicBlock* block);
	AstGraphNode* getGraphNode(Statement* node);
	
	llvm::BasicBlock* getBlockAtEntry(Statement* node);
	llvm::BasicBlock* getBlockAtExit(Statement* node);
	
	inline nodes_iterator begin()
	{
		return nodeStorage.begin();
	}
	
	inline nodes_iterator end()
	{
		return nodeStorage.end();
	}
	
	inline auto size()
	{
		return nodeStorage.size();
	}
};

class AstGraphNodeIterator
{
public:
	typedef llvm::succ_iterator BBIteratorType;
	
private:
	AstGrapher& grapher;
	BBIteratorType bbIter;
	
public:
	AstGraphNodeIterator(AstGrapher& grapher, BBIteratorType iter);
	
	AstGraphNodeIterator& operator++();
	AstGraphNode* operator*();
	bool operator==(const AstGraphNodeIterator& that) const;
	bool operator!=(const AstGraphNodeIterator& that) const;
};

template<>
struct llvm::GraphTraits<AstGraphNode>
{
	typedef AstGraphNode NodeType;
	typedef AstGraphNodeIterator ChildIteratorType;
	
	static NodeType* getEntryNode(const AstGraphNode& node);
	
	static ChildIteratorType child_begin(NodeType* node);
	static ChildIteratorType child_end(NodeType* node);
	
	typedef AstGrapher::nodes_iterator nodes_iterator;
	static nodes_iterator nodes_begin(AstGraphNode& grapher);
	static nodes_iterator nodes_end(AstGraphNode& grapher);
	
	static unsigned size(AstGrapher& grapher);
};

#endif /* ast_grapher_cpp */
