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
			typedef typename NTraits::TCRemoveReference<t_CSourceList>::CType CSourceListNoRef;
			typedef typename
				TCTypeListp_CromoteLValueRefImp
				<
					TCTypeList
					<
						tp_CCTypes...
						, typename TCChooseType
						<
							NTraits::TCIsLValueReference<t_CSourceList>::mc_Value
							, typename NTraits::TCAddLValueReference<typename TCTypeList_Get<t_Current, CSourceListNoRef>::CType>::CType
#if 1
							, typename TCTypeList_Get<t_Current, CSourceListNoRef>::CType
#else
							, typename TCChooseType
							<
								NTraits::TCIsRValueReference<t_CSourceList>::mc_Value
								, typename NTraits::TCAddRValueReference<typename TCTypeList_Get<t_Current, CSourceListNoRef>::CType>::CType
								, typename TCTypeList_Get<t_Current, CSourceListNoRef>::CType
							>::CType
#endif
						>::CType
					>
					, t_CSourceList
					, t_Current+1
					, t_End
				>::CType CType
			;
		};

		template <typename... tp_CCTypes, typename t_CSourceList, mint t_End>
		struct TCTypeListp_CromoteLValueRefImp<TCTypeList<tp_CCTypes...>, t_CSourceList, t_End, t_End>
		{
			typedef TCTypeList<tp_CCTypes...> CType;
		};
	}

	template <typename t_CSourceList, mint t_End = TCTypeList_Len<typename NTraits::TCRemoveReference<t_CSourceList>::CType>::mc_Value, mint t_Start = 0>
		requires
		(
			t_Start <= t_End // t_Start needs to be less than end
			&& t_End <= TCTypeList_Len<typename NTraits::TCRemoveReference<t_CSourceList>::CType>::mc_Value // t_End is larger than size of list
		)
	struct TCTypeList_MakePromotedLValueRef
	{
		typedef typename NPrivate::TCTypeListp_CromoteLValueRefImp<TCTypeList<>, t_CSourceList, t_Start, t_End>::CType CType;
	};

	/////////////
	// Type list is convertible


	namespace NPrivate
	{
		template <bool t_bSameLength, typename t_CFromList, typename t_CToList>
		struct TCTypeList_IsConvertibleImp : public NTraits::CCompileTimeFalse
		{
		};

		template <typename t_CFromHead, typename... tp_CFromTypes, typename t_CToHead, typename... tp_CToTypes>
		struct TCTypeList_IsConvertibleImp<true, TCTypeList<t_CFromHead, tp_CFromTypes...>, TCTypeList<t_CToHead, tp_CToTypes...>>
			: public NTraits::TCCompileTimeConstant
			<
				bool
				, NTraits::TCIsConvertible<t_CFromHead, t_CToHead>::mc_Value
				&& TCTypeList_IsConvertibleImp<true, TCTypeList<tp_CFromTypes...>, TCTypeList<tp_CToTypes...> >::mc_Value
			>
		{
		};

		template <>
		struct TCTypeList_IsConvertibleImp<true, TCTypeList<>, TCTypeList<>>
			: public NTraits::CCompileTimeTrue
		{
		};

	}

	template <typename t_CFromList, typename t_CToList>
	struct TCTypeList_IsConvertible
		: public NPrivate::TCTypeList_IsConvertibleImp
		<
			TCTypeList_Len<typename NTraits::TCRemoveReference<t_CFromList>::CType>::mc_Value
			== TCTypeList_Len<typename NTraits::TCRemoveReference<t_CToList>::CType>::mc_Value // Same length
			, typename TCTypeList_MakePromotedLValueRef<t_CFromList>::CType
			, t_CToList
		>
	{
		static_assert(!NTraits::TCIsReference<t_CToList>::mc_Value, "To list cannot be reference");
	};

	/////////////
	// Type list is constructible

	namespace NPrivate
	{
		template <bool t_bSameLength, typename t_CFromList, typename t_CToList>
		struct TCTypeList_IsConstructibleImp : public NTraits::CCompileTimeFalse
		{
		};

		template <typename t_CFromHead, typename... tp_CFromTypes, typename t_CToHead, typename... tp_CToTypes>
		struct TCTypeList_IsConstructibleImp<true, TCTypeList<t_CFromHead, tp_CFromTypes...>, TCTypeList<t_CToHead, tp_CToTypes...>>
			: public NTraits::TCCompileTimeConstant
			<
				bool
				, NTraits::TCIsConstructorCallableWith<t_CToHead, void (t_CFromHead)>::mc_Value
				&& TCTypeList_IsConstructibleImp<true, TCTypeList<tp_CFromTypes...>, TCTypeList<tp_CToTypes...> >::mc_Value
			>
		{
		};

		template <>
		struct TCTypeList_IsConstructibleImp<true, TCTypeList<>, TCTypeList<>>
			: public NTraits::CCompileTimeTrue
		{
		};

	}

	template <typename t_CFromList, typename t_CToList>
	struct TCTypeList_IsConstructible
		: public NPrivate::TCTypeList_IsConstructibleImp
		<
			TCTypeList_Len<typename NTraits::TCRemoveReference<t_CFromList>::CType>::mc_Value
			== TCTypeList_Len<typename NTraits::TCRemoveReference<t_CToList>::CType>::mc_Value // Same length
			, typename TCTypeList_MakePromotedLValueRef<t_CFromList>::CType
			, t_CToList
		>
	{
		static_assert(!NTraits::TCIsReference<t_CToList>::mc_Value, "To list cannot be reference");
	};

	/////////////
	// Type list is assignable

	namespace NPrivate
	{
		template <bool t_bSameLength, typename t_CFromList, typename t_CToList>
		struct TCTypeList_IsAssignableImp : public NTraits::CCompileTimeFalse
		{
		};

		template <typename t_CFromHead, typename... tp_CFromTypes, typename t_CToHead, typename... tp_CToTypes>
		struct TCTypeList_IsAssignableImp<true, TCTypeList<t_CFromHead, tp_CFromTypes...>, TCTypeList<t_CToHead, tp_CToTypes...>>
			: public NTraits::TCCompileTimeConstant
			<
				bool
				, NTraits::TCIsOperatorCallableWith_Assign<t_CToHead, void (t_CFromHead)>::mc_Value
				&& TCTypeList_IsAssignableImp<true, TCTypeList<tp_CFromTypes...>, TCTypeList<tp_CToTypes...> >::mc_Value
			>
		{
		};

		template <>
		struct TCTypeList_IsAssignableImp<true, TCTypeList<>, TCTypeList<>>
			: public NTraits::CCompileTimeTrue
		{
		};

	}

	template <typename t_CFromList, typename t_CToList>
	struct TCTypeList_IsAssignable
		: public NPrivate::TCTypeList_IsAssignableImp
		<
			TCTypeList_Len<typename NTraits::TCRemoveReference<t_CFromList>::CType>::mc_Value
			== TCTypeList_Len<typename NTraits::TCRemoveReference<t_CToList>::CType>::mc_Value // Same length
			, typename TCTypeList_MakePromotedLValueRef<t_CFromList>::CType
			, t_CToList
		>
	{
		static_assert(!NTraits::TCIsReference<t_CToList>::mc_Value, "To list cannot be reference");
	};
}
