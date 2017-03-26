#include <fstream>
#include <iostream>
#include <iterator>
#include <sys/file.h>
#include <unistd.h>

#include "cgrapher.hpp"

namespace
{
    FunctionsCollector::FunctionsCollector(clang::CompilerInstance & __CI, const std::string & __root, const std::string & __lock, const std::string & __out) : CI(__CI), root(__root), lock(__lock), out(__out) { }

    void FunctionsCollector::getCaller(clang::FunctionDecl * decl)
    {
        std::cout << "ok" << std::endl;
        if (decl->isThisDeclarationADefinition() && !decl->isDeleted())
        {
            std::string s;
            llvm::raw_string_ostream out(s);
            const clang::PrintingPolicy & policy = CI.getASTContext().getPrintingPolicy();
            decl->getNameForDiagnostic(out, policy, true);
            out << '(';
            
            bool first = true;
            for (auto && parameter : decl->parameters())
            {
                if (!first)
                {
                    out << ", ";
                }
                
                else
                {
                    first = false;
                }
                out << parameter->getOriginalType().getAsString(policy);
            }
            out << ')';
            std::cout << "It comes to here";
            std::cout << out.str();
            // we just init a mapping here, the callee's name will be push into vector in handleCallExpr
            std::vector<std::string> calleenames;
            result[out.str()] = calleenames;
        }
    }

    void FunctionsCollector::handleCallExpr(clang::CallExpr * callexpr)
    {
        std::string callee = "";

        clang::QualType t = callexpr->getType();
        const clang::Type * calleeType = t.getTypePtrOrNull();
        
        // TO_DO it would be better if we can get the linenumber and file name where
        // the invocation happened.
        if (calleeType != NULL)
        {
            clang::FunctionDecl *del = callexpr->getDirectCallee();
            // get the callee's name
            // TO-DO get its paramaters
            const std::string calleename = del->getNameInfo().getAsString();

            callee = t.getAsString() + " " + calleename;  
        }
        // Add the callee name into the vector
        if (result.rbegin() != result.rend())
        {
            result[result.rbegin()->first].push_back(callee);
        }
    }

    bool FunctionsCollector::visitFunctionDecl(clang::FunctionDecl * decl)
    {
        std::cout << "the function decl is running";
        getCaller(decl);

        return true;
    }

    bool FunctionsCollector::visitCallExpr(clang::CallExpr * callexpr)
    {
        std::cout << "the call expr is running";
        handleCallExpr(callexpr);

        return true;
    }

    void FunctionsCollector::print_info(std::ostream & os)
    {
        for (auto && r : result )
        {
            std::cout << r.first << " -> ";
            for (auto && c : result[r.first])
            {
                std::cout << c << '\n';
                if (c != result[r.first].back())
                {
                    os << r.first << "->";
                }
            }
        }
        std::cout << std::endl;
    }
    
    void FunctionsCollector::push_info()
    {
        if (out.empty())
        {
            print_info(std::cout);
        }
        else if (lock.empty())
        {
            std::ofstream os(out, std::ios_base::app);
            print_info(os);
            os.close();
        }
        else
        {
            const int fd = open(lock.c_str(), O_RDONLY);
            const int s = flock(fd, LOCK_EX);
            if (s == 0)
            {
                std::ofstream os(out, std::ios_base::app);
                print_info(os);
                
                os.close();

                flock(fd, LOCK_UN);
                close(fd);
            }
        }
        result.clear();
    }
    FunctionsCollectorConsumer::FunctionsCollectorConsumer(clang::CompilerInstance & __CI, const std::string & root, const std::string & lock, const std::string & out) : clang::ASTConsumer(), CI(__CI), visitor(__CI, root, lock, out) { }
    
    FunctionsCollectorConsumer::~FunctionsCollectorConsumer() { }

    void FunctionsCollectorConsumer::HandleTranslationUnit(clang::ASTContext & ctxt)
    {
        visitor.TraverseDecl(ctxt.getTranslationUnitDecl());
        visitor.push_info();
    }

    bool FunctionsCollectorConsumer::shouldSkipFunctionBody(clang::Decl * decl)
    {
        return false;
    }

    std::unique_ptr<clang::ASTConsumer> FunctionsCollectorAction::CreateASTConsumer(clang::CompilerInstance &CI, llvm::StringRef)
    {
        return llvm::make_unique<FunctionsCollectorConsumer>(CI, root, lock, out);
    }

    bool FunctionsCollectorAction::ParseArgs(const clang::CompilerInstance & CI, const std::vector<std::string> & args)
    {
        if (args.size() >= 1)
        {
            root = args[0];
            if (args.size() >= 2)
            {
                out = args[1];
                if (args.size() >= 3)
                {
                    lock = args[2];
                }
            }
        }
        
        return true;
    }

        // Automatically run the plugin after the main AST action
        clang::PluginASTAction::ActionType FunctionsCollectorAction::getActionType()
        {
            return AddAfterMainAction;
        }
        
        static clang::FrontendPluginRegistry::Add<FunctionsCollectorAction> X("cgrapher", "get a call graph");
}

