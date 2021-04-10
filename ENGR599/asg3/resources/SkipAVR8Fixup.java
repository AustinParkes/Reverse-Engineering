
//For an AVR8 binary, adds references to skip instructions to correct the Function Graph viewer
//@category Processor.AVR8

import ghidra.app.script.GhidraScript;
import ghidra.program.model.address.*;
import ghidra.program.model.listing.Instruction;
import ghidra.program.model.listing.InstructionIterator;
import ghidra.program.model.symbol.*;
import util.CollectionUtils;

public class SkipAVR8Fixup extends GhidraScript {

	@Override
	public void run() throws Exception {

		if (currentProgram == null) {
			return;
		}

		AddressSetView set = currentProgram.getMemory().getLoadedAndInitializedAddressSet();
		if (currentSelection != null && !currentSelection.isEmpty()) {
			set = new AddressSet(currentSelection);
		}

		InstructionIterator instructions = currentProgram.getListing().getInstructions(set, true);

		// For each instruction in the selection
        while (instructions.hasNext()) {
			Instruction instrL = instructions.next();
			// Look for a reference to one of the skip instructions
			if (!CollectionUtils.isOneOf(instrL.getMnemonicString(), "cpse", "sbis", "sbic", "sbrc",
				"sbrs")) {
				continue;
			}

			// Jump instructions following skip instructions already treated as conditional branches
			Instruction nextInst = instrL.getNext();
			if (CollectionUtils.isOneOf(nextInst.getMnemonicString(), "jmp", "rjmp", "ijmp", "eijmp")) {
				continue;
			}

			Instruction refInst = instrL.getNext().getNext();
			Address refAddr = refInst.getMinAddress();
			Address instAddr = instrL.getMinAddress();

			// If a reference from the current instruction to the subsequent instruction does not exist
			if (!referenceExists(instAddr, refAddr)) {
				printf("Added reference at %s\n", instAddr.toString(true));
				// Add a reference
				instrL.addMnemonicReference(refAddr, RefType.CONDITIONAL_JUMP,
					SourceType.USER_DEFINED);
			}
			else {
				printf("Reference at %s already exists\n", instAddr.toString(true));
			}
		}
	}

	boolean referenceExists(Address fromAddr, Address toAddr) {
		Reference[] refs = currentProgram.getReferenceManager().getReferencesFrom(fromAddr);
		for (Reference ref : refs) {
			if (ref.getToAddress().equals(toAddr)) {
				return true;
			}
		}
		return false;
	}
}
