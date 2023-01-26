// Copyright © 2015 Hansoft AB 
// Distributed under the MIT license, see license text in LICENSE.Malterlib

#pragma once

namespace NMib::NMeta
{
	/////////////
	// Indices

	template <mint...>
	struct TCIndices
	{
	};

	namespace NPrivate
	{
		template <mint t_CurrentIndex, typename t_CIndices, mint t_EndIndex>
		struct TCMakeConsecutiveIndicesImp;

		template <mint t_CurrentIndex, mint... _Indices, mint t_EndIndex>
		struct TCMakeConsecutiveIndicesImp<t_CurrentIndex, TCIndices<_Indices...>, t_EndIndex>
		{
			typedef typename TCMakeConsecutiveIndicesImp<t_CurrentIndex+1, TCIndices<_Indices..., t_CurrentIndex>, t_EndIndex>::CType CType;
		};

		template <mint t_EndIndex, mint... _Indices>
		struct TCMakeConsecutiveIndicesImp<t_EndIndex, TCIndices<_Indices...>, t_EndIndex>
		{
			typedef TCIndices<_Indices...> CType;
		};
	}

	template <mint t_EndIndex, mint t_StartIndex = 0>
		requires (t_StartIndex <= t_EndIndex) // Start index cannot be larger than the end index
	struct TCMakeConsecutiveIndices
	{
		typedef typename NPrivate::TCMakeConsecutiveIndicesImp<t_StartIndex, TCIndices<>, t_EndIndex>::CType CType;
	};

	/////////////
	// Integer sequence

	template <typename t_CInteger, t_CInteger ...t_Integer>
	struct TCIntegerSequence
	{
		static constexpr mint mc_Size = sizeof...(t_Integer);
	};

	template <mint t_Index, typename t_CIntegerSequence>
	struct TCIntegerSequence_Get;

	template <mint t_Index, typename t_CInteger>
	struct TCIntegerSequence_Get<t_Index, TCIntegerSequence<t_CInteger>>
	{
		static_assert(t_Index == 0 && t_Index != 0, "TCIntegerSequence_Get index out of range");
	};

	template <typename t_CInteger, t_CInteger t_Head, t_CInteger ...t_Integer>
	struct TCIntegerSequence_Get<0, NMeta::TCIntegerSequence<t_CInteger, t_Head, t_Integer...>>
	{
		static constexpr t_CInteger mc_Value = t_Head;
	};

	template <mint t_Index, typename t_CInteger, t_CInteger t_Head, t_CInteger ...t_Integer>
	struct TCIntegerSequence_Get<t_Index, TCIntegerSequence<t_CInteger, t_Head, t_Integer...>>
	{
		static constexpr t_CInteger mc_Value = TCIntegerSequence_Get<t_Index-1, TCIntegerSequence<t_CInteger, t_Integer...>>::mc_Value;
	};

	/////////////
	// All is true

	namespace NPrivate
	{
		template <bool... tp_CToCheck> struct TCAllIsTrueImp;

		template <bool t_bToCheck, bool... tp_CToCheck>
		struct TCAllIsTrueImp<t_bToCheck, tp_CToCheck...>
		{
			static constexpr bool mc_Value = t_bToCheck && TCAllIsTrueImp<tp_CToCheck...>::mc_Value;
		};

		template <>
		struct TCAllIsTrueImp<>
		{
			static constexpr bool mc_Value = true;
		};
	}

	template <bool... tp_CToCheck>
	struct TCAllIsTrue
		: public NTraits::TCCompileTimeConstant<bool, NPrivate::TCAllIsTrueImp<tp_CToCheck...>::mc_Value>
	{
	};


	/////////////
	// Type list

	template <typename... tp_CCTypes>
	struct TCTypeList
	{
	};

	/////////////
	// Type list concat

	template <typename t_CList0, typename t_CList1>
	struct TCTypeList_Concat;

	template <typename... tp_CParams0, typename... tp_CParams1>
	struct TCTypeList_Concat<TCTypeList<tp_CParams0...>, TCTypeList<tp_CParams1...>>
	{
		typedef TCTypeList<tp_CParams0..., tp_CParams1...> CType;
	};

	/////////////
	// Type list len

	template <typename t_CType>
	struct TCTypeList_Len;

	// Support any qualifiers
	template <typename t_CType>
	struct TCTypeList_Len<t_CType const>
		: public TCTypeList_Len<t_CType>
	{
	};
	template <typename t_CType>
	struct TCTypeList_Len<t_CType volatile>
		: public TCTypeList_Len<t_CType>
	{
	};
	template <typename t_CType>
	struct TCTypeList_Len<t_CType const volatile>
		: public TCTypeList_Len<t_CType>
	{
	};

	template <typename... tp_CCTypes>
	struct TCTypeList_Len<TCTypeList<tp_CCTypes...>>
		: public NTraits::TCCompileTimeConstant<mint, sizeof...(tp_CCTypes)>
	{
	};

	/////////////
	// Type list get

	template <mint t_Index, typename t_CType>
	struct TCTypeList_Get;

	// Copy qualifiers of type list
	template <mint t_Index, typename t_CType>
	struct TCTypeList_Get<t_Index, t_CType const>
	{
		typedef typename NTraits::TCAddConst<typename TCTypeList_Get<t_Index, t_CType>::CType>::CType CType;
	};
	template <mint t_Index, typename t_CType>
	struct TCTypeList_Get<t_Index, t_CType volatile>
	{
		typedef typename NTraits::TCAddVolatile<typename TCTypeList_Get<t_Index, t_CType>::CType>::CType CType;
	};
	template <mint t_Index, typename t_CType>
	struct TCTypeList_Get<t_Index, t_CType const volatile>
	{
		typedef typename NTraits::TCAddConstVolatile<typename TCTypeList_Get<t_Index, t_CType>::CType>::CType CType;
	};

	template <mint t_Index>
	struct TCTypeList_Get<t_Index, TCTypeList<> >
	{
		static_assert(t_Index == 0 && t_Index != 0, "TCTypeList_Get index out of range");
	};

	template <typename t_CHeadType, typename... tp_CCTypes>
	struct TCTypeList_Get<0, NMeta::TCTypeList<t_CHeadType, tp_CCTypes...>>
	{
		typedef t_CHeadType CType;
	};

	template <mint t_Index, typename t_CHeadType, typename... tp_CCTypes>
	struct TCTypeList_Get<t_Index, TCTypeList<t_CHeadType, tp_CCTypes...>>
	{
		typedef typename TCTypeList_Get<t_Index-1, TCTypeList<tp_CCTypes...>>::CType CType;
	};


	template <mint t_Index, typename t_CType>
	struct TCTypeList_GetOrVoid;

	// Copy qualifiers of type list
	template <mint t_Index, typename t_CType>
	struct TCTypeList_GetOrVoid<t_Index, t_CType const>
	{
		typedef typename NTraits::TCAddConst<typename TCTypeList_GetOrVoid<t_Index, t_CType>::CType>::CType CType;
	};
	template <mint t_Index, typename t_CType>
	struct TCTypeList_GetOrVoid<t_Index, t_CType volatile>
	{
		typedef typename NTraits::TCAddVolatile<typename TCTypeList_GetOrVoid<t_Index, t_CType>::CType>::CType CType;
	};
	template <mint t_Index, typename t_CType>
	struct TCTypeList_GetOrVoid<t_Index, t_CType const volatile>
	{
		typedef typename NTraits::TCAddConstVolatile<typename TCTypeList_GetOrVoid<t_Index, t_CType>::CType>::CType CType;
	};

	template <mint t_Index>
	struct TCTypeList_GetOrVoid<t_Index, TCTypeList<> >
	{
		typedef void CType;
	};

	template <typename t_CHeadType, typename... tp_CCTypes>
	struct TCTypeList_GetOrVoid<0, NMeta::TCTypeList<t_CHeadType, tp_CCTypes...>>
	{
		typedef t_CHeadType CType;
	};

	template <mint t_Index, typename t_CHeadType, typename... tp_CCTypes>
	struct TCTypeList_GetOrVoid<t_Index, TCTypeList<t_CHeadType, tp_CCTypes...>>
	{
		typedef typename TCTypeList_GetOrVoid<t_Index-1, TCTypeList<tp_CCTypes...>>::CType CType;
	};

	namespace NPrivate
	{
		template <typename t_CInput>
		struct TCTemplateClassBase;

		template <template <typename ...> class t_TCTemplate, typename ...tp_CArgs>
		struct TCTemplateClassBase<t_TCTemplate<tp_CArgs...>>
		{
			using CType = t_TCTemplate<>;
		};
	}

	template
	<
		typename t_CInput
		, typename = typename NPrivate::TCTemplateClassBase<t_CInput>::CType
	>
	struct TCReverseTemplateArguments;

	template
	<
		template <typename...> class t_TCTemplate
		, typename ...tp_CArgs
	>
	struct TCReverseTemplateArguments
	<
		typename NPrivate::TCTemplateClassBase<t_TCTemplate<tp_CArgs...>>::CType
		, t_TCTemplate<tp_CArgs...>
	>
	{
		using CType = t_TCTemplate<tp_CArgs...>;
	};

	template
	<
		template <typename...> class t_TCTemplate
		, typename t_CFirst
		, typename ...tp_CRest
		, typename ...tp_CDone
	>
	struct TCReverseTemplateArguments
	<
		t_TCTemplate<t_CFirst, tp_CRest...>
		, t_TCTemplate<tp_CDone...>
	>
	{
		using CType = typename TCReverseTemplateArguments<t_TCTemplate<tp_CRest...>, t_TCTemplate<t_CFirst, tp_CDone...>>::CType;
	};
}

namespace NMib
{
	template <typename... tfp_CTypes>
	void fg_Swallow(tfp_CTypes &&...) noexcept
	{
	}
}

#ifndef DMibPNoShortCuts
	using namespace NMib::NMeta;
#endif
