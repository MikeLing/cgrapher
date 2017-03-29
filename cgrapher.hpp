#ifndef __CGRAPHER_HPP__
#define __CGRAPHER_HPP__

#include <ostream>
#include <string>
#include <vector>
#include <map>

#include "clang/Frontend/FrontendPluginRegistry.h"
#include "clang/AST/AST.h"
#include "clang/AST/ASTConsumer.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Sema/Sema.h"
#include "llvm/Support/raw_ostream.h"

#include "clang/Analysis/CallGraph.h"

namespace
{

    class GraphGeneratorConsume : public clang::ASTConsumer
    {
        clang::CompilerInstance & CI;
        clang::CallGraph visitor;

        public:
            GraphGeneratorConsume(clang::CompilerInstance & __CI);
            virtual ~GraphGeneratorConsume();

            virtual void HandleTranslationUnit(clang::ASTContext & ctxt);
            virtual bool shouldSkipFunctionBody(clang::Decl * decl);
    };

    class GraphGeneratoAction : public clang::PluginASTAction
    {
        protected:
            std::unique_ptr<clang::ASTConsumer> CreateASTConsumer(clang::CompilerInstance &CI, llvm::StringRef) override;
            bool ParseArgs(const clang::CompilerInstance & CI, const std::vector<std::string> & args) override;

            // Automatically run the plugin after the main AST action
            PluginASTAction::ActionType getActionType() override;
    };
}

#endif //___CGRAPHER_HPP__
