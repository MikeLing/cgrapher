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

namespace
{
    class FunctionsCollector : public clang::RecursiveASTVisitor<FunctionsCollector>
    {
        clang::CompilerInstance & CI;
        const std::string root;
        const std::string lock;
        const std::string out;

        // the  output like assumen like "callername->calleename | file | line number"
        std::map<std::string, std::vector<std::string>> result;

        public:
            FunctionsCollector(clang::CompilerInstance &_CI, const std::string & __root, const std::string & __lock, const std::string & __out);
            
            // Get caller's infomation
            void getCaller(clang::FunctionDecl * decl);

            // Visit the callee node and return its name
            void handleCallExpr(clang::CallExpr * callexpr);
            bool visitFunctionDecl(clang::FunctionDecl * decl);
            bool visitCallExpr(clang::CallExpr * callexpr);
            void push_info();
            void print_info(std::ostream & os);

    };

    class FunctionsCollectorConsumer : public clang::ASTConsumer
    {
        clang::CompilerInstance & CI;
        FunctionsCollector visitor;

        public:
            FunctionsCollectorConsumer(clang::CompilerInstance & __CI, const std::string & root, const std::string & lock, const std::string & out);
            virtual ~FunctionsCollectorConsumer();

            virtual void HandleTranslationUnit(clang::ASTContext & ctxt);
            virtual bool shouldSkipFunctionBody(clang::Decl * decl);
    };

    class FunctionsCollectorAction : public clang::PluginASTAction
    {
        std::string root;
        std::string lock;
        std::string out;

        protected:
            std::unique_ptr<clang::ASTConsumer> CreateASTConsumer(clang::CompilerInstance &CI, llvm::StringRef) override;
            bool ParseArgs(const clang::CompilerInstance & CI, const std::vector<std::string> & args) override;

            // Automatically run the plugin after the main AST action
            PluginASTAction::ActionType getActionType() override;
    };
}

#endif //___CGRAPHER_HPP__
