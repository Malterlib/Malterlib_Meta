// Copyright © 2015 Hansoft AB 
// Distributed under the MIT license, see license text in LICENSE.Malterlib

#pragma once
#include "Malterlib_Meta.h"

namespace NMib::NMeta
{
	/////////////
	// Type list promote lvalue ref

	namespace NPrivate
	{
		template <typename t_CTypes, typename t_CSourceList, mint t_Current, mint t_End>
		struct TCTypeListp_CromoteLValueRefImp;

		template <typename... tp_CCTypes, typename t_CSourceList, mint t_Current, mint t_End>
		struct TCTypeListp_CromoteLValueRefImp<TCTypeList<tp_CCTypes...>, t_CSourceList, t_Current, t_End>
		{
			using CSourceListNoRef = NTraits::TCRemoveReference<t_CSourceList>;
			using CType = typename
				TCTypeListp_CromoteLValueRefImp
				<
					TCTypeList
					<
						tp_CCTypes...
						, TCConditional
						<
							NTraits::cIsLValueReference<t_CSourceList>
							, NTraits::TCAddLValueReference<TCTypeList_Get<t_Current, CSourceListNoRef>>
#if 1
							, TCTypeList_Get<t_Current, CSourceListNoRef>
#else
							, TCConditional
							<
								NTraits::cIsRValueReference<t_CSourceList>
								, NTraits::TCAddRValueReference<TCTypeList_Get<t_Current, CSourceListNoRef>>
								, TCTypeList_Get<t_Current, CSourceListNoRef>
							>
#endif
						>
					>
					, t_CSourceList
					, t_Current+1
					, t_End
				>::CType
			;
		};

		template <typename... tp_CCTypes, typename t_CSourceList, mint t_End>
		struct TCTypeListp_CromoteLValueRefImp<TCTypeList<tp_CCTypes...>, t_CSourceList, t_End, t_End>
		{
			using CType = TCTypeList<tp_CCTypes...>;
		};

		template <typename t_CSourceList, mint t_End = gc_TypeList_Len<NTraits::TCRemoveReference<t_CSourceList>>, mint t_Start = 0>
		struct TCTypeList_MakePromotedLValueRef
		{
			using CType = typename NPrivate::TCTypeListp_CromoteLValueRefImp<TCTypeList<>, t_CSourceList, t_Start, t_End>::CType;
		};
	}

	template <typename t_CSourceList, mint t_End = gc_TypeList_Len<NTraits::TCRemoveReference<t_CSourceList>>, mint t_Start = 0>
		requires
		(
			t_Start <= t_End // t_Start needs to be less than end
			&& t_End <= gc_TypeList_Len<NTraits::TCRemoveReference<t_CSourceList>> // t_End is larger than size of list
		)
	using TCTypeList_MakePromotedLValueRef = typename NPrivate::TCTypeList_MakePromotedLValueRef<t_CSourceList, mint t_End, t_Start>::CType;

	/////////////
	// Type list is convertible


	namespace NPrivate
	{
		template <bool t_bSameLength, typename t_CFromList, typename t_CToList>
		struct TCTypeList_IsConvertibleImp
		{
			constexpr static bool mc_bValue = false;
		};

		template <typename t_CFromHead, typename... tp_CFromTypes, typename t_CToHead, typename... tp_CToTypes>
		struct TCTypeList_IsConvertibleImp<true, TCTypeList<t_CFromHead, tp_CFromTypes...>, TCTypeList<t_CToHead, tp_CToTypes...>>
		{
			constexpr static bool mc_bValue = NTraits::cIsConvertible<t_CFromHead, t_CToHead>
				&& TCTypeList_IsConvertibleImp<true, TCTypeList<tp_CFromTypes...>, TCTypeList<tp_CToTypes...>>::mc_bValue
			;
		};

		template <>
		struct TCTypeList_IsConvertibleImp<true, TCTypeList<>, TCTypeList<>>
		{
			constexpr static bool mc_bValue = true;
		};

		template <typename t_CFromList, typename t_CToList>
		struct TCTypeList_IsConvertible
			: public NPrivate::TCTypeList_IsConvertibleImp
			<
				gc_TypeList_Len<NTraits::TCRemoveReference<t_CFromList>> == gc_TypeList_Len<NTraits::TCRemoveReference<t_CToList>> // Same length
				, TCTypeList_MakePromotedLValueRef<t_CFromList>
				, t_CToList
			>
		{
			static_assert(!NTraits::cIsReference<t_CToList>, "To list cannot be reference");
		};
	}

	template <typename t_CFromList, typename t_CToList>
	concept cTypeList_IsConvertible = NPrivate::TCTypeList_IsConvertible<t_CFromList, t_CToList>::mc_bValue;

	/////////////
	// Type list is constructible

	namespace NPrivate
	{
		template <bool t_bSameLength, typename t_CFromList, typename t_CToList>
		struct TCTypeList_IsConstructibleImp
		{
			constexpr static bool mc_bValue = false;
		};

		template <typename t_CFromHead, typename... tp_CFromTypes, typename t_CToHead, typename... tp_CToTypes>
		struct TCTypeList_IsConstructibleImp<true, TCTypeList<t_CFromHead, tp_CFromTypes...>, TCTypeList<t_CToHead, tp_CToTypes...>>
		{
			constexpr static bool mc_bValue = NTraits::cIsConstructibleWith<t_CToHead, t_CFromHead>
				&& TCTypeList_IsConstructibleImp<true, TCTypeList<tp_CFromTypes...>, TCTypeList<tp_CToTypes...> >::mc_bValue
			;
		};

		template <>
		struct TCTypeList_IsConstructibleImp<true, TCTypeList<>, TCTypeList<>>
		{
			constexpr static bool mc_bValue = true;
		};

	}

	template <typename t_CFromList, typename t_CToList>
	struct TCTypeList_IsConstructible
		: public NPrivate::TCTypeList_IsConstructibleImp
		<
			gc_TypeList_Len<NTraits::TCRemoveReference<t_CFromList>> == gc_TypeList_Len<NTraits::TCRemoveReference<t_CToList>> // Same length
			, TCTypeList_MakePromotedLValueRef<t_CFromList>
			, t_CToList
		>
	{
		static_assert(!NTraits::cIsReference<t_CToList>, "To list cannot be reference");
	};

	/////////////
	// Type list is assignable

	namespace NPrivate
	{
		template <bool t_bSameLength, typename t_CFromList, typename t_CToList>
		struct TCTypeList_IsAssignableImp
		{
			constexpr static bool mc_bValue = false;
		};

		template <typename t_CFromHead, typename... tp_CFromTypes, typename t_CToHead, typename... tp_CToTypes>
		struct TCTypeList_IsAssignableImp<true, TCTypeList<t_CFromHead, tp_CFromTypes...>, TCTypeList<t_CToHead, tp_CToTypes...>>
		{
			constexpr static bool mc_bValue = NTraits::cIsAssignableWith<t_CToHead, t_CFromHead>
				&& TCTypeList_IsAssignableImp<true, TCTypeList<tp_CFromTypes...>, TCTypeList<tp_CToTypes...> >::mc_bValue
			;
		};

		template <>
		struct TCTypeList_IsAssignableImp<true, TCTypeList<>, TCTypeList<>>
		{
			constexpr static bool mc_bValue = true;
		};
	}

	template <typename t_CFromList, typename t_CToList>
	struct TCTypeList_IsAssignable
		: public NPrivate::TCTypeList_IsAssignableImp
		<
			gc_TypeList_Len<NTraits::TCRemoveReference<t_CFromList>> == gc_TypeList_Len<NTraits::TCRemoveReference<t_CToList>> // Same length
			, TCTypeList_MakePromotedLValueRef<t_CFromList>
			, t_CToList
		>
	{
		static_assert(!NTraits::cIsReference<t_CToList>, "To list cannot be reference");
	};
}
