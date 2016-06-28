package polyllvm.ast.PseudoLLVM.Statements;

import polyglot.ast.Ext;
import polyglot.util.CodeWriter;
import polyglot.util.Position;
import polyglot.util.SerialVersionUID;
import polyglot.visit.PrettyPrinter;
import polyllvm.ast.PseudoLLVM.Expressions.LLVMLabel;

public class LLVMSeqLabel_c extends LLVMInstruction_c
        implements LLVMSeqLabel, LLVMLabel {
    private static final long serialVersionUID = SerialVersionUID.generate();

    protected String name;

    public LLVMSeqLabel_c(Position pos, String name, Ext e) {
        super(pos, e);
        this.name = name;
    }

    @Override
    public String name() {
        return name;
    }

    @Override
    public void prettyPrint(CodeWriter w, PrettyPrinter pp) {
        w.write(name + ":");
    }

}