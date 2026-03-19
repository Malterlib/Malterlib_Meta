// Copyright © 2015 Hansoft AB
// Distributed under the MIT license, see license text in LICENSE.Malterlib

#pragma once

namespace NMib::NMeta
{
	/////////////
	// Indices

	template <umint...>
	struct TCIndices
	{
	};

	namespace NPrivate
	{
		template <umint t_CurrentIndex, typename t_CIndices, umint t_EndIndex>
		struct TCMakeConsecutiveIndicesImp;

		template <umint t_CurrentIndex, umint... _Indices, umint t_EndIndex>
		struct TCMakeConsecutiveIndicesImp<t_CurrentIndex, TCIndices<_Indices...>, t_EndIndex>
		{
			using CType = typename TCMakeConsecutiveIndicesImp<t_CurrentIndex+1, TCIndices<_Indices..., t_CurrentIndex>, t_EndIndex>::CType;
		};

		template <umint t_EndIndex, umint... _Indices>
		struct TCMakeConsecutiveIndicesImp<t_EndIndex, TCIndices<_Indices...>, t_EndIndex>
		{
			using CType = TCIndices<_Indices...>;
		};
	}

	template <umint t_EndIndex, umint t_StartIndex = 0>
		requires (t_StartIndex <= t_EndIndex) // Start index cannot be larger than the end index
	using TCConsecutiveIndices = typename NPrivate::TCMakeConsecutiveIndicesImp<t_StartIndex, TCIndices<>, t_EndIndex>::CType;

	/////////////
	// Integer sequence

	template <typename t_CInteger, t_CInteger ...t_Integer>
	struct TCIntegerSequence
	{
		static constexpr umint mc_Size = sizeof...(t_Integer);
	};

	namespace NPrivate
	{
		template <umint t_Index, typename t_CIntegerSequence>
		struct TCIntegerSequence_Get;

		template <umint t_Index, typename t_CInteger>
		struct TCIntegerSequence_Get<t_Index, TCIntegerSequence<t_CInteger>>
		{
			static_assert(t_Index == 0 && t_Index != 0, "TCIntegerSequence_Get index out of range");
		};

		template <typename t_CInteger, t_CInteger t_Head, t_CInteger ...t_Integer>
		struct TCIntegerSequence_Get<0, NMeta::TCIntegerSequence<t_CInteger, t_Head, t_Integer...>>
		{
			static constexpr t_CInteger mc_Value = t_Head;
		};

		template <umint t_Index, typename t_CInteger, t_CInteger t_Head, t_CInteger ...t_Integer>
		struct TCIntegerSequence_Get<t_Index, TCIntegerSequence<t_CInteger, t_Head, t_Integer...>>
		{
			static constexpr t_CInteger mc_Value = TCIntegerSequence_Get<t_Index-1, TCIntegerSequence<t_CInteger, t_Integer...>>::mc_Value;
		};
	}

	template <umint t_Index, typename t_CIntegerSequence>
	inline constexpr umint gc_IntegerSequence_Get = NPrivate::TCIntegerSequence_Get<t_Index, t_CIntegerSequence>::mc_Value;

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
	concept cAllIsTrue = (tp_CToCheck && ...);

	/////////////
	// Type list

	template <typename... tp_CCTypes>
	struct TCTypeList
	{
	};

	/////////////
	// Type list concat

	namespace NPrivate
	{
		template <typename t_CList0, typename t_CList1>
		struct TCTypeList_Concat;

		template <typename... tp_CParams0, typename... tp_CParams1>
		struct TCTypeList_Concat<TCTypeList<tp_CParams0...>, TCTypeList<tp_CParams1...>>
		{
			using CType = TCTypeList<tp_CParams0..., tp_CParams1...>;
		};
	}

	template <typename t_CList0, typename t_CList1>
	using TCTypeList_Concat = typename NPrivate::TCTypeList_Concat<t_CList0, t_CList1>::CType;

	/////////////
	// Type list len

	namespace NPrivate
	{
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
		{
			constexpr static umint mc_Value = sizeof...(tp_CCTypes);
		};
	}

	template <typename t_CType>
	inline constexpr umint gc_TypeList_Len = NPrivate::TCTypeList_Len<t_CType>::mc_Value;

	/////////////
	// Type list get

	namespace NPrivate
	{
		template <umint t_Index, typename t_CType>
		struct TCTypeList_Get;

		// Copy qualifiers of type list
		template <umint t_Index, typename t_CType>
		struct TCTypeList_Get<t_Index, t_CType const>
		{
			using CType = NTraits::TCAddConst<typename TCTypeList_Get<t_Index, t_CType>::CType>;
		};
		template <umint t_Index, typename t_CType>
		struct TCTypeList_Get<t_Index, t_CType volatile>
		{
			using CType = NTraits::TCAddVolatile<typename TCTypeList_Get<t_Index, t_CType>::CType>;
		};
		template <umint t_Index, typename t_CType>
		struct TCTypeList_Get<t_Index, t_CType const volatile>
		{
			using CType = NTraits::TCAddConstVolatile<typename TCTypeList_Get<t_Index, t_CType>::CType>;
		};

		template <umint t_Index>
		struct TCTypeList_Get<t_Index, TCTypeList<> >
		{
			static_assert(t_Index == 0 && t_Index != 0, "TCTypeList_Get index out of range");
		};

		template <typename t_CHeadType, typename... tp_CCTypes>
		struct TCTypeList_Get<0, NMeta::TCTypeList<t_CHeadType, tp_CCTypes...>>
		{
			using CType = t_CHeadType;
		};

		template <umint t_Index, typename t_CHeadType, typename... tp_CCTypes>
		struct TCTypeList_Get<t_Index, TCTypeList<t_CHeadType, tp_CCTypes...>>
		{
			using CType = typename TCTypeList_Get<t_Index-1, TCTypeList<tp_CCTypes...>>::CType;
		};
	}

	template <umint t_Index, typename t_CType>
	using TCTypeList_Get = typename NPrivate::TCTypeList_Get<t_Index, t_CType>::CType;

	namespace NPrivate
	{
		template <umint t_Index, typename t_CType>
		struct TCTypeList_GetOrVoid;

		// Copy qualifiers of type list
		template <umint t_Index, typename t_CType>
		struct TCTypeList_GetOrVoid<t_Index, t_CType const>
		{
			using CType = NTraits::TCAddConst<typename TCTypeList_GetOrVoid<t_Index, t_CType>::CType>;
		};
		template <umint t_Index, typename t_CType>
		struct TCTypeList_GetOrVoid<t_Index, t_CType volatile>
		{
			using CType = NTraits::TCAddVolatile<typename TCTypeList_GetOrVoid<t_Index, t_CType>::CType>;
		};
		template <umint t_Index, typename t_CType>
		struct TCTypeList_GetOrVoid<t_Index, t_CType const volatile>
		{
			using CType = NTraits::TCAddConstVolatile<typename TCTypeList_GetOrVoid<t_Index, t_CType>::CType>;
		};

		template <umint t_Index>
		struct TCTypeList_GetOrVoid<t_Index, TCTypeList<> >
		{
			using CType = void;
		};

		template <typename t_CHeadType, typename... tp_CCTypes>
		struct TCTypeList_GetOrVoid<0, NMeta::TCTypeList<t_CHeadType, tp_CCTypes...>>
		{
			using CType = t_CHeadType;
		};

		template <umint t_Index, typename t_CHeadType, typename... tp_CCTypes>
		struct TCTypeList_GetOrVoid<t_Index, TCTypeList<t_CHeadType, tp_CCTypes...>>
		{
			using CType = typename TCTypeList_GetOrVoid<t_Index-1, TCTypeList<tp_CCTypes...>>::CType;
		};
	}

	template <umint t_Index, typename t_CType>
	using TCTypeList_GetOrVoid = typename NPrivate::TCTypeList_GetOrVoid<t_Index, t_CType>::CType;

	namespace NPrivate
	{
		template <typename t_CInput>
		struct TCTemplateClassBase;

		template <template <typename ...> class t_TCTemplate, typename ...tp_CArgs>
		struct TCTemplateClassBase<t_TCTemplate<tp_CArgs...>>
		{
			using CType = t_TCTemplate<>;
		};

		template
		<
			typename t_CInput
			, typename = typename TCTemplateClassBase<t_CInput>::CType
		>
		struct TCReverseTemplateArguments;

		template
		<
			template <typename...> class t_TCTemplate
			, typename ...tp_CArgs
		>
		struct TCReverseTemplateArguments
		<
			typename TCTemplateClassBase<t_TCTemplate<tp_CArgs...>>::CType
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

	template <typename t_CInput>
	using TCReverseTemplateArguments = typename NPrivate::TCReverseTemplateArguments<t_CInput>::CType;
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
