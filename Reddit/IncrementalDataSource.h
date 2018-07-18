#include <vector>
#include <functional>
namespace Reddit
{
	public ref class IncrementalDataSource sealed: Windows::UI::Xaml::Interop::IBindableObservableVector, Windows::UI::Xaml::Data::ISupportIncrementalLoading
	{
	private:
		Platform::Collections::Vector<Platform::Object^> % vec;
	internal:
		IncrementalDataSource(Platform::Collections::Vector<Platform::Object^> % v);
	public:
		// Inherited via IBindableVector
		virtual Windows::UI::Xaml::Interop::IBindableIterator ^ First();

		// Inherited via IBindableObservableVector
		virtual property unsigned int Size
		{
			unsigned int get()
			{
				return vec.Size;
			}
		}
		virtual Platform::Object ^ GetAt(unsigned int index);
		virtual Windows::UI::Xaml::Interop::IBindableVectorView ^ GetView();
		virtual bool IndexOf(Platform::Object ^value, unsigned int *index);
		virtual void SetAt(unsigned int index, Platform::Object ^value);
		virtual void InsertAt(unsigned int index, Platform::Object ^value);
		virtual void RemoveAt(unsigned int index);
		virtual void Append(Platform::Object ^value);
		virtual void RemoveAtEnd();
		virtual void Clear();
		virtual event Windows::UI::Xaml::Interop::BindableVectorChangedEventHandler ^ VectorChanged;

		// Inherited via ISupportIncrementalLoading
		virtual property bool HasMoreItems
		{
			bool get()
			{
				__debugbreak();
				return false;
			}
		}
		virtual Windows::Foundation::IAsyncOperation<Windows::UI::Xaml::Data::LoadMoreItemsResult> ^ LoadMoreItemsAsync(unsigned int count);
	};
}