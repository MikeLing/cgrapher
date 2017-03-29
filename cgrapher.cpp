#include <fstream>
#include <iostream>
#include <iterator>
#include <sys/file.h>
#include <unistd.h>

#include "cgrapher.hpp"

namespace
{
    GraphGeneratorConsume::GraphGeneratorConsume(clang::CompilerInstance & __CI) : 
            clang::ASTConsumer(), CI(__CI) { }
    
    GraphGeneratorConsume::~GraphGeneratorConsume() { }

    void GraphGeneratorConsume::HandleTranslationUnit(clang::ASTContext & ctxt)
    {
        visitor.TraverseDecl(ctxt.getTranslationUnitDecl());
        visitor.dump();
    }

    bool GraphGeneratorConsume::shouldSkipFunctionBody(clang::Decl * decl)
    {
        return false;
    }

    std::unique_ptr<clang::ASTConsumer> GraphGeneratoAction::CreateASTConsumer(clang::CompilerInstance &CI, llvm::StringRef)
    {
        return llvm::make_unique<GraphGeneratorConsume>(CI);
    }

    bool GraphGeneratoAction::ParseArgs(const clang::CompilerInstance & CI, const std::vector<std::string> & args)
    {
        return true;
    }

        // Automatically run the plugin after the main AST action
        clang::PluginASTAction::ActionType GraphGeneratoAction::getActionType()
        {
            return AddAfterMainAction;
        }
        
        static clang::FrontendPluginRegistry::Add<GraphGeneratoAction> X("cgrapher", "get a call graph");
}

