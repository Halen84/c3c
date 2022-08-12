static inline LLVMValueRef llvm_emit_insert_value(GenContext *c, LLVMValueRef agg, LLVMValueRef new_value, ArraySize index)
{
	if (LLVMGetTypeKind(LLVMTypeOf(agg)) == LLVMVectorTypeKind)
	{
		LLVMValueRef index_val = llvm_const_int(c, type_usize, index);
		return LLVMBuildInsertElement(c->builder, agg, new_value, index_val, "");
	}
	return LLVMBuildInsertValue(c->builder, agg, new_value, index, "");
}

INLINE LLVMValueRef llvm_store_decl(GenContext *c, Decl *decl, BEValue *value)
{
	BEValue ref;
	llvm_value_set_decl(c, &ref, decl);
	assert(llvm_value_is_addr(&ref));
	return llvm_store(c, &ref, value);
}

INLINE LLVMValueRef llvm_store_raw(GenContext *c, BEValue *destination, LLVMValueRef raw_value)
{
	assert(llvm_value_is_addr(destination));
	return llvm_store_to_ptr_raw_aligned(c, destination->value, raw_value, destination->alignment);
}


INLINE LLVMValueRef llvm_store_decl_raw(GenContext *context, Decl *decl, LLVMValueRef value)
{
	assert(!decl->is_value);
	return llvm_store_to_ptr_raw_aligned(context, decl->backend_ref, value, decl->alignment);
}

INLINE AlignSize llvm_type_or_alloca_align(GenContext *c, LLVMValueRef dest, Type *type)
{
	if (LLVMIsAAllocaInst(dest) || LLVMIsAGlobalVariable(dest))
	{
		return LLVMGetAlignment(dest);
	}
	return type_abi_alignment(type);
}

INLINE LLVMValueRef llvm_store_to_ptr(GenContext *c, LLVMValueRef destination, BEValue *value)
{
	return llvm_store_to_ptr_aligned(c, destination, value, llvm_type_or_alloca_align(c, destination, value->type));
}

INLINE LLVMValueRef llvm_store_to_ptr_raw(GenContext *c, LLVMValueRef pointer, LLVMValueRef value, Type *type)
{
	return llvm_store_to_ptr_raw_aligned(c, pointer, value, llvm_type_or_alloca_align(c, pointer, type));
}

static inline LLVMValueRef llvm_emit_bitcast(GenContext *c, LLVMValueRef value, Type *type)
{
	assert(type->type_kind == TYPE_POINTER);
	LLVMTypeRef result_type = llvm_get_type(c, type);
	if (result_type == LLVMTypeOf(value)) return value;
	return LLVMBuildBitCast(c->builder, value, result_type, "");
}

INLINE void llvm_value_bitcast(GenContext *c, BEValue *value, Type *type)
{
	assert(llvm_value_is_addr(value));
	type = type_lowering(type);
	value->value = llvm_emit_bitcast(c, value->value, type_get_ptr(type));
	value->type = type;
}

INLINE LLVMValueRef llvm_emit_bitcast_ptr(GenContext *c, LLVMValueRef value, Type *type)
{
	return llvm_emit_bitcast(c, value, type_get_ptr(type));
}

INLINE LLVMValueRef llvm_emit_shl(GenContext *c, LLVMValueRef value, LLVMValueRef shift)
{
	return LLVMBuildShl(c->builder, value, shift, "shl");
}

INLINE LLVMValueRef llvm_emit_ashr(GenContext *c, LLVMValueRef value, LLVMValueRef shift)
{
	return LLVMBuildAShr(c->builder, value, shift, "ashr");
}

INLINE LLVMValueRef llvm_emit_lshr(GenContext *c, LLVMValueRef value, LLVMValueRef shift)
{
	return LLVMBuildLShr(c->builder, value, shift, "lshrl");
}

INLINE LLVMValueRef llvm_emit_trunc_bool(GenContext *c, LLVMValueRef value)
{
	return LLVMBuildTrunc(c->builder, value, c->bool_type, "");
}

INLINE LLVMValueRef llvm_emit_trunc(GenContext *c, LLVMValueRef value, Type *type)
{
	return LLVMBuildTrunc(c->builder, value, llvm_get_type(c, type), "");
}

INLINE bool llvm_use_debug(GenContext *context) { return context->debug.builder != NULL; }

INLINE bool llvm_basic_block_is_unused(LLVMBasicBlockRef block)
{
	return !LLVMGetFirstInstruction(block) && !LLVMGetFirstUse(LLVMBasicBlockAsValue(block));
}

static inline LLVMBasicBlockRef llvm_get_current_block_if_in_use(GenContext *context)
{
	LLVMBasicBlockRef block = context->current_block;
	if (llvm_basic_block_is_unused(block))
	{
		LLVMDeleteBasicBlock(block);
		context->current_block = NULL;
		context->current_block_is_target = false;
		return NULL;
	}
	return block;
}

INLINE bool call_supports_variadic(CallABI abi)
{
	switch (abi)
	{
		case CALL_X86_STD:
		case CALL_X86_REG:
		case CALL_X86_THIS:
		case CALL_X86_FAST:
		case CALL_X86_VECTOR:
			return false;
		default:
			return true;

	}
}

static inline LLVMCallConv llvm_call_convention_from_call(CallABI abi)
{
	switch (abi)
	{
		case CALL_C:
			return LLVMCCallConv;
		case CALL_X86_STD:
			return LLVMX86StdcallCallConv;
		case CALL_X86_FAST:
			return LLVMX86FastcallCallConv;
		case CALL_X86_REG:
			return LLVMX86RegCallCallConv;
		case CALL_X86_THIS:
			return LLVMX86ThisCallCallConv;
		case CALL_X86_VECTOR:
			return LLVMX86VectorCallCallConv;
		case CALL_AAPCS:
			return LLVMARMAAPCSCallConv;
		case CALL_AAPCS_VFP:
			return LLVMARMAAPCSVFPCallConv;
		default:
			return LLVMCCallConv;
	}

}

INLINE bool llvm_is_global_eval(GenContext *c)
{
	return c->builder == c->global_builder;
}

INLINE bool llvm_is_local_eval(GenContext *c)
{
	return c->builder != c->global_builder;
}


INLINE LLVMTypeRef llvm_get_ptr_type(GenContext *c, Type *type)
{
	return llvm_get_type(c, type_get_ptr(type));
}

INLINE LLVMValueRef llvm_emit_and(GenContext *c, BEValue *lhs, BEValue *rhs)
{
	return LLVMBuildAnd(c->builder, lhs->value, rhs->value, "");
}

INLINE LLVMValueRef llvm_get_zero(GenContext *c, Type *type)
{
	return LLVMConstNull(llvm_get_type(c, type));
}

INLINE LLVMValueRef llvm_get_zero_raw(LLVMTypeRef type)
{
	return LLVMConstNull(type);
}

INLINE LLVMValueRef llvm_get_undef(GenContext *c, Type *type)
{
	return LLVMGetUndef(llvm_get_type(c, type));
}

INLINE LLVMValueRef llvm_get_undef_raw(LLVMTypeRef type)
{
	return LLVMGetUndef(type);
}

INLINE LLVMValueRef llvm_const_int(GenContext *c, Type *type, uint64_t val)
{
	type = type_lowering(type);
	assert(type_is_integer(type) || type->type_kind == TYPE_BOOL);
	return LLVMConstInt(llvm_get_type(c, type), val, type_is_integer_signed(type));
}

INLINE LLVMValueRef llvm_add_global(GenContext *c, const char *name, Type *type, AlignSize alignment)
{
	return llvm_add_global_raw(c, name, llvm_get_type(c, type_lowering(type_no_optional(type))), alignment);
}

INLINE LLVMValueRef llvm_add_global_raw(GenContext *c, const char *name, LLVMTypeRef type, AlignSize alignment)
{
	LLVMValueRef ref = LLVMAddGlobal(c->module, type, name);
	LLVMSetAlignment(ref, (unsigned)alignment ? alignment : LLVMPreferredAlignmentOfGlobal(c->target_data, ref));
	return ref;
}

INLINE void llvm_emit_exprid(GenContext *c, BEValue *value, ExprId expr)
{
	assert(expr);
	llvm_emit_expr(c, value, exprptr(expr));
}


INLINE void llvm_set_alignment(LLVMValueRef alloca, AlignSize alignment)
{
	assert(alignment > 0);
	LLVMSetAlignment(alloca, (unsigned)alignment);
}

INLINE void llvm_emit_statement_chain(GenContext *c, AstId current)
{
	while (current)
	{
		llvm_emit_stmt(c, ast_next(&current));
	}
}