package polyllvm.types;

import polyglot.ext.jl5.types.JL5LocalInstance;

public interface PolyLLVMLocalInstance extends JL5LocalInstance {

    /**
     * Whether this is a compiler-generated variable that should not be
     * visible to the user when debugging.
     */
    boolean isTemp();

    /** Whether this variable is in SSA form. */
    boolean isSSA();
}
