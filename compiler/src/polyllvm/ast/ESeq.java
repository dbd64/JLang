package polyllvm.ast;

import polyglot.ast.Expr;
import polyglot.ast.Stmt;

import java.util.List;

public interface ESeq extends Expr {

    List<Stmt> statements();

    Expr expr();
}